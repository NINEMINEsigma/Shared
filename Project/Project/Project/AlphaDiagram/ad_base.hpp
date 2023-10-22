#pragma once

#ifndef __AlphaDiagram

#define __AlphaDiagram

#include<typeinfo>
#include<type_traits>

#include<functional>
#include<mutex>
#include<queue>
#include<map>
#include<future>
#include<vector>

namespace ad
{
#pragma region 成员检测

	template <class...>
	struct make_void
	{
		using type = void;
	};

	template <typename... T>
	using void_t = typename make_void<T...>::type;

	template <class T, class = void>
	struct has_foo : std::false_type {};

	//template <class T>
	//struct has_foo<T, void_t<decltype(std::declval<T&>().foo())>> : std::true_type {};

	//template <class T>
	//inline constexpr bool has_foo_v = has_foo<T>::value;

#define def_has_function(T,name)															\
	struct has_foo<T, void_t<decltype(std::declval<T&>().name())>> : std::true_type {};		\
																							\
	inline constexpr bool is_has_function_##name = has_foo<T>::value;						\

#define def_has_member(T,name)																\
	struct has_member<T, void_t<decltype(std::declval<T&>().name)>> : std::true_type {};	\
																							\
	inline constexpr bool is_has_member_##name = has_member<T>::value;						\

#pragma endregion

#pragma region STL容器检测

	template <typename T>
	struct is_stl_array_like_impl
	{
		using type = std::remove_const_t<T>;

		template <typename A>
		static constexpr bool check(
			A* pt,
			A const* cpt = nullptr,
			decltype(pt->begin())* = nullptr,
			decltype(pt->end())* = nullptr,
			decltype(cpt->begin())* = nullptr,
			decltype(cpt->end())* = nullptr,
			typename A::iterator* pi = nullptr,
			typename A::const_iterator* pci = nullptr,
			typename A::value_type* pv = nullptr)
		{
			using iterator = typename A::iterator;
			using const_iterator = typename A::const_iterator;
			using value_type = typename A::value_type;
			return std::is_same<decltype(pt->begin()), iterator>::value&&
				std::is_same<decltype(pt->end()), iterator>::value&&
				std::is_same<decltype(cpt->begin()), const_iterator>::value&&
				std::is_same<decltype(cpt->end()), const_iterator>::value&&
				std::is_same<decltype(**pi), value_type&>::value&&
				std::is_same<decltype(**pci), value_type const&>::value;
		}

		template <typename A>
		static constexpr bool check(...)
		{
			return false;
		}

		static const bool value = check<type>(nullptr);
	};

#pragma endregion

#pragma region 函数参数检测

	template <class T, typename... Arguments>
	struct wrapper
	{
		using result_type = decltype(std::declval<T>().bar(std::declval<Arguments>()...));
		using type = std::conditional_t<true, result_type, std::integral_constant<result_type(T::*)(Arguments...), &T::bar>>;
	};

	template <class T, typename... Arguments>
	using has_para_t = typename wrapper<T, Arguments...>::type;

	/*

	struct test
	{
	int foo(int &&)
	{
		return 0;
	}
	};

	static_assert(std::experimental::is_detected_v<has_para_t, foo1, int &&>, "int&& detected");
	*/

#pragma endregion

#pragma region 对象哈希值获取

	class universal_hash
	{
	public:
		template <typename... Types>
		static size_t hash_val(const Types&... args);

		template <typename T, typename... Types>
		static void hash_value(size_t& seed,
			const T& firstArg,
			const Types&... args);

		template <typename T>
		static void hash_value(size_t& seed,
			const T& val);

		template<typename T>
		static void hash_combine(size_t& seed,
			const T& val);
	};

	template<typename ...Types>
	size_t universal_hash::hash_val(const Types & ...args)
	{
		size_t seed = 0;
		hash_value(seed, args...);
		return seed;
	}

	template<typename T, typename ...Types>
	void universal_hash::hash_value(size_t& seed, const T& firstArg, const Types & ...args)
	{
		hash_combine(seed, firstArg);
		hash_value(seed, args...);
	}

	template<typename T>
	void universal_hash::hash_value(size_t& seed, const T& val)
	{
		hash_combine(seed, val);
	}

	template<typename T>
	void universal_hash::hash_combine(size_t& seed, const T& val)
	{
		seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

#pragma endregion

#pragma region thread_pool

	template<class _T>
	class thread_queue
	{
	public:
		bool empty()
		{
			std::unique_lock<std::mutex> lock(_m_mutex);
			return _m_queue.empty();
		}
		size_t size()
		{
			std::unique_lock<std::mutex> lock(_m_mutex);
			return _m_queue.size();
		}
		void enqueue(_T& _Right)
		{
			std::unique_lock<std::mutex> lock(_m_mutex);
			_m_queue.emplace(_Right);
		}
		bool dequeue(_T& _Right)
		{
			std::unique_lock<std::mutex> lock(_m_mutex);
			if (_m_queue.empty())return false;
			_Right = std::move(_m_queue.front());
			_m_queue.pop();
			return true;
		}

	private:
		std::queue<_T> _m_queue;
		std::mutex _m_mutex;
	};

	class thread_pool
	{
		friend class threadStream;
	public:
		thread_pool(const int _n_threads = 4)
			: _m_threads(std::vector<std::thread>(_n_threads))
		{
		}
		thread_pool(const thread_pool&) = delete;
		thread_pool(thread_pool&&) = delete;
		thread_pool& operator=(const thread_pool&) = delete;
		thread_pool& operator=(thread_pool&&) = delete;

		// Submit a function to be executed asynchronously by the pool
		template <typename F, typename... Args>
		auto submit(F&& f, Args &&...args) -> std::future<decltype(f(args...))>
		{
			// Create a function with bounded parameter ready to execute
			// 连接函数和参数定义，特殊函数类型，避免左右值错误
			std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
			// Encapsulate it into a shared pointer in order to be able to copy construct
			auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

			// Warp packaged task into void function
			std::function<void()> warpper_func = [task_ptr]()
			{
				(*task_ptr)();
			};

			// 队列通用安全封包函数，并压入安全队列
			_m_queue.enqueue(warpper_func);

			// 唤醒一个等待中的线程
			_m_conditional_lock.notify_one();

			// 返回先前注册的任务指针
			return task_ptr->get_future();
		}

		// Submit a function to be executed asynchronously by the pool
		template <typename F, typename... Args>
		auto operator<<(std::pair< F&&, Args &&...> arg) -> std::future<decltype(arg.first(arg.second))>
		{
			return submit(arg.first, arg.second);
		}

		// Inits thread pool
		void init()
		{
			for (size_t i = 0, e = _m_threads.size(); i < e; ++i)
			{
				_m_threads.at(i) = std::thread(threadStream(this, (int)i)); // 分配工作线程
			}
		}

		// Waits until threads finish their current task and shutdowns the pool
		void shutdown()
		{
			_m_shutdown = true;
			_m_conditional_lock.notify_all(); // 通知，唤醒所有工作线程
			for (int i = 0; i < _m_threads.size(); ++i)
			{
				if (_m_threads.at(i).joinable()) // 判断线程是否在等待
				{
					_m_threads.at(i).join(); // 将线程加入到等待队列
				}
			}
		}

		operator bool()
		{
			return _m_shutdown;
		}

		class threadStream // 内置线程工作类
		{
		private:
			int m_id; // 工作id
			thread_pool* m_pool; // 所属线程池
		public:
			threadStream(_In_ thread_pool* pool, const int id) : m_pool(pool), m_id(id)
			{

			}

			void operator()()
			{
				std::function<void()> func;
				bool dequeued; // 是否正在取出队列中元素
				// 判断线程池是否关闭，没有关闭则从任务队列中循环提取任务
				if (m_pool != nullptr)
					while (!*m_pool)
					{
						{
							// 为线程环境加锁，互访问工作线程的休眠和唤醒
							std::unique_lock<std::mutex> lock(m_pool->_m_conditional_mutex);
							// 如果任务队列为空，阻塞当前线程
							if (m_pool->_m_queue.empty())
							{
								m_pool->_m_conditional_lock.wait(lock); // 等待条件变量通知，开启线程
							}
							// 取出任务队列中的元素
							dequeued = m_pool->_m_queue.dequeue(func);
						}
						// 如果成功取出，执行工作函数
						if (dequeued)
							func();
					}
			}
		};

	private:
		std::condition_variable _m_conditional_lock;// 线程环境锁，可以让线程处于休眠或者唤醒状态
		std::mutex _m_conditional_mutex;// 线程休眠锁互斥变量
		std::vector<std::thread> _m_threads; // 工作线程队列
		thread_queue<std::function<void()>> _m_queue;// 执行函数安全队列，即任务队列
		bool _m_shutdown = false;
	};

#pragma endregion

	enum class error_exception
	{
		error_operator = 1 << 0,
		error_generate = 1 << 1
	};

	bool ___is_generate_operator_by_object_system = false;
	std::mutex __generate_operator_by_object_system_lock;

	class _Base
	{
	protected:
		_Base()
		{
			if (!___is_generate_operator_by_object_system)throw error_exception::error_operator;
		}
	public:
		virtual ~_Base()
		{
		}

		const std::type_info& get_type()const noexcept
		{
			return typeid(*this);
		}
		size_t get_hash() const noexcept
		{
			return universal_hash::hash_val(this);
		}

	private:
		size_t _m_instance_index = 0;
	};
	
	template<class T>
	class _BaseGenerator
	{
		static_assert(std::is_base_of_v<_Base, T>, "Invalid template parameters");
	public:
		template<typename ..._Args>
		T* generate(_Args... args)
		{
			T* cat = nullptr;
			___is_generate_operator_by_object_system = true;
			{
				std::unique_lock<std::mutex> lock(__generate_operator_by_object_system_lock);
				cat = new T(args);
			}
			___is_generate_operator_by_object_system = false;
			init(cat);
			return cat;
		}

		bool release(T* _Right)
		{
			auto temp = std::find(_m_line.begin(), _m_line.end(), [&_Right](T* L) {return L == _Right; });
			if (temp != _m_line.end())
			{
				_m_line.erase(temp);
				_m_out_line.push_back(_Right);
				return true;
			}
			else return false;
		}

		bool pull_in(T* _Right)
		{
			auto temp = std::find(_m_out_line.begin(), _m_out_line.end(), [&_Right](T* L) {return L == _Right; });
			if (temp != _m_out_line.end())
			{
				_m_out_line.erase(temp);
				_m_line.push_back(_Right);
				return true;
			}
			else return false;
		}

		bool delete_immediately(T* _Right)
		{
			{
				auto temp = std::find(_m_line.begin(), _m_line.end(), [&_Right](T* L) {return L == _Right; });
				if (temp != _m_line.end())
				{
					_m_line.erase(temp);
					delete _Right;
					return true;
				}
			}
			{
				auto temp = std::find(_m_out_line.begin(), _m_out_line.end(), [&_Right](T* L) {return L == _Right; });
				if (temp != _m_out_line.end())
				{
					_m_out_line.erase(temp);
					delete _Right;
					return true;
				}
			}
		}

		std::vector<T*>  _m_out_line;

	protected:
		virtual void init(T* _Right) abstract;
	private:
		std::vector<T*> _m_line;
	};

}

#endif // !__AlphaDiagram
