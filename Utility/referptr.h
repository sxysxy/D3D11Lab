#pragma once
/*
	Easy Smart Pointer based on reference count.
				--by HfCloud.
*/

#include <assert.h>

namespace Utility{


class ReferredObject{
public:
	int __ref_count;
	void *__ptr_marker;
	
	ReferredObject(){
		__ref_count = 0;
		__ptr_marker = nullptr;
	}
	
	int AddRefer(){
		return ++__ref_count;
	}
	
	int SubRefer(){
		return --__ref_count;
	}
	
	int GetReferCount() const{
		return __ref_count;
	}
	
	virtual void Release() = 0;
};

template<typename _T>
class ReferPtr {
	typedef _T *_pT;
	typedef _pT *_ppT;

	_pT _ptr;
public:
	ReferPtr() {
		_ptr = nullptr;
	}
	ReferPtr(const _pT _optr) :ReferPtr() {
		(*this) = _optr;
	}
	ReferPtr(const ReferPtr &_optr) :ReferPtr() {
		(*this) = _optr;
	}
	~ReferPtr() {
		Release();
	}

	_pT Get() const {
		return _ptr;
	}
	_ppT GetAddressOf() const {
		return &_ptr;
	}
	_ppT GetAddressOfAndRelease() {
		Release();
		return GetAddressOf();
	}
	_pT operator->() const {
		assert(_ptr);
		
		return _ptr;
	}
    const _T &operator*() const {
        assert(_ptr);

        return (*_ptr);
    }

	void Release() {
		if(_ptr == nullptr)return;
		if(_ptr -> __ptr_marker)_ptr -> SubRefer();
		else{
			_ptr -> __ptr_marker = (void *)this;
			_ptr -> Release();
			if(_ptr -> SubRefer() == 0){
				delete _ptr;
			}else if(_ptr -> __ptr_marker == (void *)this){
				_ptr -> __ptr_marker = nullptr;
				return;
			}
		}
		_ptr = nullptr;
	}

	explicit operator bool() const {
		Get() != nullptr;
	}
	_ppT operator&() {
		return GetAddressOfAndRelease();
	}

	_pT operator=(const _pT _optr) {
		Release();
		_ptr = _optr;
		_ptr->AddRefer();
		return _ptr;
	}
	const ReferPtr &operator=(const ReferPtr &_optr) {
		Release();
		_ptr = _optr.Get();
		_ptr->AddRefer();
		return _optr;
	}

	bool operator==(const ReferPtr &_optr) {
		return _ptr == _optr.Get();
	}
	bool operator==(const _pT _optr) {
		return _ptr == _optr;
	}

	bool operator!=(const _pT _optr) {
		return (*this) != _optr;
	}
	bool operator!=(const ReferPtr &_optr) {
		return (*this) != _optr;
	}

	template<typename ... _Arg>
	static ReferPtr New(const _Arg & ..._arg) {
		typedef _T ___T;          //mdzz msvc++ _T __T 
		ReferPtr _ptr = new ___T(_arg...);
		return _ptr;
	}
};

}
