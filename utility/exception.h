#ifndef USING_EXCEPTION_H
#define USING_EXCEPTION_H

///
/// exception class thrown by OFEC
///
namespace OFEC {
	class exception {
	public :
		exception(const char* message)  { m_message = message; }
		~exception() noexcept {}
		const char* what() const noexcept { return m_message; }
	private :
		const char* m_message;
	};
}
#endif                            // end of USING_EXCEPTION_H

