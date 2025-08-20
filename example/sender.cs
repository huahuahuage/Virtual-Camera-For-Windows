using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

public class Startup
{
    // Windows API函数声明
    [DllImport("kernel32.dll", SetLastError = true)]
    static extern IntPtr OpenFileMapping(int dwDesiredAccess, bool bInheritHandle, string lpName);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern IntPtr CreateFileMapping(IntPtr hFile, IntPtr lpAttributes, uint flProtect,
        uint dwMaximumSizeHigh, uint dwMaximumSizeLow, string lpName);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern IntPtr MapViewOfFile(IntPtr hFileMapping, uint dwDesiredAccess,
        uint dwFileOffsetHigh, uint dwFileOffsetLow, uint dwNumberOfBytesToMap);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern bool UnmapViewOfFile(IntPtr lpBaseAddress);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern bool CloseHandle(IntPtr hObject);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern IntPtr CreateEvent(IntPtr lpEventAttributes, bool bManualReset,
        bool bInitialState, string lpName);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern bool SetEvent(IntPtr hEvent);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern IntPtr CreateMutex(IntPtr lpMutexAttributes, bool bInitialOwner, string lpName);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern bool ReleaseMutex(IntPtr hMutex);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern uint WaitForSingleObject(IntPtr hHandle, uint dwMilliseconds);

    // 常量定义
    private const int FILE_MAP_ALL_ACCESS = 0xF001F;
    private const uint PAGE_READWRITE = 0x04;
    private const uint FILE_MAP_WRITE = 0x0002;
    private const uint EVENT_MODIFY_STATE = 0x0002;
    private const int HEADER_SIZE = 32;
    private const int MAX_SHARED_IMAGE_SIZE = 3840 * 2160 * 4 * 2;
    private const int RECEIVE_MAX_WAIT = 200;
    private const uint WAIT_OBJECT_0 = 0;
    private const uint WAIT_TIMEOUT = 0x102;

    // 共享内存和事件句柄
    private static IntPtr _hMapFile = IntPtr.Zero;
    private static IntPtr _pBuf = IntPtr.Zero;
    private static IntPtr _hSentEvent = IntPtr.Zero;
    private static IntPtr _hWantEvent = IntPtr.Zero;
    private static IntPtr _hMutex = IntPtr.Zero;
    private static bool _isInitialized = false;
    private static readonly object _lockObj = new object();

    public Startup()
    {
        Initialize(); // 实例化时自动执行
    }


    // 初始化共享资源
    private bool Initialize()
    {
        lock (_lockObj)
        {
            if (_isInitialized)
            {
                return true;
            }

            // Console.WriteLine("Initialized");

            try
            {
                // 创建共享内存
                _hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, "HuahualiveCapture_Data");
                if (_hMapFile == IntPtr.Zero)
                {
                    _hMapFile = CreateFileMapping(new IntPtr(-1), IntPtr.Zero, PAGE_READWRITE, 0, (uint)(HEADER_SIZE + MAX_SHARED_IMAGE_SIZE), "HuahualiveCapture_Data");
                }

                if (_hMapFile == IntPtr.Zero)
                    return false;

                // 映射视图
                _pBuf = MapViewOfFile(_hMapFile, FILE_MAP_WRITE, 0, 0,
                    (uint)(HEADER_SIZE + MAX_SHARED_IMAGE_SIZE));
                if (_pBuf == IntPtr.Zero)
                    return false;

                // 创建事件和互斥体
                _hSentEvent = CreateEvent(IntPtr.Zero, false, false, "HuahualiveCapture_Sent");
                _hWantEvent = CreateEvent(IntPtr.Zero, false, false, "HuahualiveCapture_Want");
                _hMutex = CreateMutex(IntPtr.Zero, false, "HuahualiveCapture_Mutx");

                if (_hSentEvent == IntPtr.Zero || _hWantEvent == IntPtr.Zero || _hMutex == IntPtr.Zero)
                    return false;

                // 初始化共享内存头
                Marshal.WriteInt32(_pBuf, 0, MAX_SHARED_IMAGE_SIZE);
                Marshal.WriteInt32(_pBuf, 28, int.MaxValue - RECEIVE_MAX_WAIT);

                _isInitialized = true;
                return true;
            }
            catch
            {
                Cleanup();
                return false;
            }
        }
    }

    // 清理资源
    private void Cleanup()
    {
        if (_pBuf != IntPtr.Zero)
        {
            UnmapViewOfFile(_pBuf);
            _pBuf = IntPtr.Zero;
        }
        if (_hMapFile != IntPtr.Zero)
        {
            CloseHandle(_hMapFile);
            _hMapFile = IntPtr.Zero;
        }
        if (_hSentEvent != IntPtr.Zero)
        {
            CloseHandle(_hSentEvent);
            _hSentEvent = IntPtr.Zero;
        }
        if (_hWantEvent != IntPtr.Zero)
        {
            CloseHandle(_hWantEvent);
            _hWantEvent = IntPtr.Zero;
        }
        if (_hMutex != IntPtr.Zero)
        {
            CloseHandle(_hMutex);
            _hMutex = IntPtr.Zero;
        }
        _isInitialized = false;
    }

    // 发送帧数据
    public async Task<object> Invoke(dynamic input)
    {
        if (!Initialize())
        {
            throw new Exception("Failed to initialize virtual camera");
        }

        try
        {
            byte[] bitmap = input.bitmap;
            int width = input.width;
            int height = input.height;

            if (bitmap == null || bitmap.Length != width * height * 4)
                throw new Exception("Invalid bitmap data");

            // 使用Task.Run将图像处理放到线程池
            await Task.Run(() =>
            {
                // 预计算缓冲区大小和行大小
                int pixSize = width * height;
                int rowSize = width * 4;

                // 获取互斥体
                uint waitResult = WaitForSingleObject(_hMutex, 100);
                if (waitResult == WAIT_TIMEOUT)
                    throw new TimeoutException("Failed to acquire mutex (timeout)");
                if (waitResult != WAIT_OBJECT_0)
                    throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());

                try
                {
                    // 更新帧信息
                    Marshal.WriteInt32(_pBuf, 4, width);         // 图像的宽度
                    Marshal.WriteInt32(_pBuf, 8, height);        // 图像的高度
                    Marshal.WriteInt32(_pBuf, 12, width);    // 使用对齐后的行大小作为stride
                    Marshal.WriteInt32(_pBuf, 16, 0);            // FORMAT_UINT8
                    Marshal.WriteInt32(_pBuf, 20, 1);            // RESIZEMODE_DISABLED
                    Marshal.WriteInt32(_pBuf, 24, 0);            // MIRRORMODE_DISABLED

                    // 创建ARGB格式的缓冲区
                    byte[] processData = new byte[width * height * 4];

                    for (int i = 0; i < width * height; i++)
                    {
                        // 像素起始点
                        int srcOffset = i * 4;
                        int pixRowNumber = i / width;
                        int pixRowOffset = i % width;
                        int dstPixRowNumber = height - i / width  - 1; // 翻转图像
                        // 目标起始点
                        int dstOffset = (dstPixRowNumber * width + pixRowOffset) * 4;

                        // BGRA -> ARGB
                        processData[dstOffset++] = bitmap[srcOffset + 2];
                        processData[dstOffset++] = bitmap[srcOffset + 1];
                        processData[dstOffset++] = bitmap[srcOffset];
                        processData[dstOffset++] = bitmap[srcOffset + 3];
                    }

                    // 复制转换后的数据
                    IntPtr dataPtr = IntPtr.Add(_pBuf, HEADER_SIZE);
                    Marshal.Copy(processData, 0, dataPtr, processData.Length);

                    // 通知接收方
                    SetEvent(_hSentEvent);
                }
                finally
                {
                    ReleaseMutex(_hMutex);
                }
            });

            return "OK";
        }
        catch (Exception ex)
        {
            Cleanup();
            throw new Exception("Failed to send frame: " + ex.Message);
        }
    }
}
