#pragma once
#include <mutex>
#include "NoCopy.h"
#include "NoMove.h"

namespace utils
{
	template<typename ITEM>
	class SafeHolder: NoCopy
	{
	public:
		class LockedItem: NoCopy, NoMove
		{
		public:
			LockedItem(ITEM& ref, decltype(SafeHolder::m_mutex)& mutex)
				: m_ref(ref)
				, m_lock(mutex)
			{
			}

			ITEM* operator->()
			{
				return &m_ref;
			}

			ITEM& ref()
			{
				return m_ref;
			}

		private:
			ITEM& m_ref;
			std::unique_lock<decltype(SafeHolder::m_mutex)> m_lock;

		};

		SafeHolder() = default;
		SafeHolder(ITEM item)
			: m_item(item)
		{}

		SafeHolder(SafeHolder&& o) noexcept
		{
			*this = std::move(o);
		}

		SafeHolder& operator=(SafeHolder&& o) noexcept
		{
			o.m_mutex.lock();
			m_item = std::move(o.m_item);
			o.m_mutex.unlock();
			return *this;
		}

		LockedItem Lock()
		{
			return LockedItem(m_item, m_mutex);
		}

	private:
		ITEM m_item;
		std::mutex m_mutex;
	};
}
