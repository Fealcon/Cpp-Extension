#pragma once

/// The ScopedFreePointer is like a unique_ptr but uses free instead of delete, when leaving scope
template< typename T >
class ScopedFreePointer {
	/// the underlying pointer to the malloced space
	T *ptr;

  public:
	/// takes ownership of the malloced space at \p
	constexpr ScopedFreePointer(T *p)
		: ptr(p) {}
	/// free the space at \a p
	~ScopedFreePointer() {
		free(ptr);
	}
	/// pointer like access
	constexpr T *operator->() {
		return ptr;
	}
	/// conversion to the actual pointer
	constexpr operator T *() {
		return ptr;
	}
};
/// deduction guide to get type from constructor
template< typename T >
ScopedFreePointer(T *) -> ScopedFreePointer< T >;
