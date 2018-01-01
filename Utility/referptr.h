#pragma once
/*
	Easy Smart Pointer based on reference count.
				--by HfCloud.
*/

namespace Utility{


class ReferredObject{
	int _ref_count;
public:
	ReferredObject(){
		_ref_count = 0;
	}
	
	void AddRefer(){
		_ref_count += 1;
	}
	
	void SubRefer(){
		_ref_count -= 1;
	}
	
	int GetReferCount() const{
		return _ref_count;
	}
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
	ReferPtr(ReferPtr &_optr) :ReferPtr() {
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
		return Get();
	}

	void Release() {
		if (_ptr) {
			_ptr->SubRefer();
			if (_ptr->GetReferCount() == 0) {
				delete _ptr;
				_ptr = nullptr;
			}
		}
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
		typedef _T ___T; //mdzz msvc++ _T __T 
		ReferPtr _ptr = new ___T(_arg...);
		return _ptr;
	}
};

}