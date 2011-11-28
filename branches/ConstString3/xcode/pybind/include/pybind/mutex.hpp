#	pragma

namespace pybind
{
	class mutex
	{
	public:
		mutex();
		~mutex();

	protected:
		int m_state;
	};
}
