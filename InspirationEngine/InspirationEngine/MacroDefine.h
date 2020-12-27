#pragma once

#define KILL(_POINTER_) if(_POINTER_)delete _POINTER_; _POINTER_ = 0//포인터 초기화
#define aKILL(_POINTER_) if(_POINTER_)delete[] _POINTER_; _POINTER_ = 0//배열포인터 초기화