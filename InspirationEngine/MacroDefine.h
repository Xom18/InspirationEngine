#pragma once

#define KILL(_POINTER_) if(_POINTER_)delete _POINTER_; _POINTER_ = 0//������ �ʱ�ȭ
#define aKILL(_POINTER_) if(_POINTER_)delete[] _POINTER_; _POINTER_ = 0//�迭������ �ʱ�ȭ