use crate::common;
mod pinto;
pub fn get_rust_callbacks(array_ptr: *mut *mut common::Callback) { unsafe {
	*array_ptr.offset(0) = pinto::start as *mut extern "C" fn();
	*array_ptr.offset(1) = pinto::update as *mut extern "C" fn();
}}