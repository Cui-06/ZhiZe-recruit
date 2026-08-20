"""当前环境的 NumPy 兼容占位文件。

本机 conda 环境里的 NumPy 二进制导入会报错，但本项目的数据读取已经改成
纯 Python + torch.Tensor。PyTorch 启动时只需要少量 NumPy 类型名，所以这里
提供最小兼容别名，避免程序因为环境问题无法运行。
"""

__version__ = "0.0.0"


class _Module:
    # 模拟 numpy.fft / numpy.linalg / numpy.random 这类子模块对象。
    def __init__(self, name):
        self.__name__ = name


class ndarray(list):
    pass


class dtype:
    def __init__(self, value=None):
        self.value = value


class generic:
    pass


class number:
    pass


class object_:
    pass


integer = int
floating = float
complexfloating = complex
bool_ = bool

int8 = int16 = int32 = int64 = int
uint8 = uint16 = uint32 = uint64 = int
float16 = float32 = float64 = float
complex64 = complex128 = complex

pi = 3.141592653589793
inf = float("inf")
nan = float("nan")

fft = _Module("numpy.fft")
linalg = _Module("numpy.linalg")
random = _Module("numpy.random")


def isscalar(value):
    return isinstance(value, (str, bytes, int, float, complex, bool))


def array(value, dtype=None):
    return value


def asarray(value, dtype=None):
    return value
