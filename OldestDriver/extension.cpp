#include "stdafx.h"
#include "extension.h"
#include "HFWindow.h"
namespace Ext {

	VALUE __msgbox__(VALUE self, VALUE msg) {
		msg = rb_funcall(msg, rb_intern("to_s"), 0);
		cstring s;
		U8ToU16(rb_string_value_cstr(&msg), s);
		MessageBoxW(0, s.c_str(), L"MessageBox", MB_OK);
		return Qnil;
	}
	VALUE show_console(VALUE self) {
		AllocConsole();
		int s = 0;
		rb_eval_string_protect("STDIN.reopen('CON'); STDOUT.reopen('CON'); STDERR.reopen('CON');", &s);
		if (s) {
			rb_raise(rb_eException, "Error when reopening STDIO to this console.");
		}
		return Qnil;
	}
	VALUE dummy(VALUE self, ...) {
		return self;
	}

	void BasicExtensions() {
		rb_define_module_function(rb_mKernel, "msgbox", (rubyfunc)__msgbox__, 1);
		rb_define_module_function(rb_mKernel, "show_console", (rubyfunc)show_console, 0);

	}

	namespace DX {
		VALUE module;

		VALUE messageloop(VALUE self) {
			MSG msg;
			while (true) {
				if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else {
					if (rb_block_given_p()) {
						rb_yield(Qnil);
					}
					
				}
			}
		}

		void Init233() { //misc things...
			module = rb_define_module("DX");
			rb_define_singleton_method(module, "messageloop", (rubyfunc)messageloop, 0);
		}

	}

	void ApplyExtensions() {
		BasicExtensions();

		Ext::HFWindow::Init();
		Ext::DX::Init233();
	}
}
