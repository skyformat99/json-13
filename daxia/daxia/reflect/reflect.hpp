/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file reflect.hpp
 * \author �콭��Ĵ�Ϻ
 * \date ʮ�� 2018
 *
 * �ṩ����ʱ�ķ��书�ܣ�
 * ��ǩ�����͡��ڴ沼��
 *
 */

#ifndef __DAXIA_REFLECT_REFLECT_HPP
#define __DAXIA_REFLECT_REFLECT_HPP

#include <string>
#include <vector>
#include <daxia/reflect/reflect_base.hpp>

#define HASH "hash"
#define OFFSET "offset"
#define SIZE "size"

namespace daxia
{
	namespace reflect
	{
		template<class ValueType>
		class reflect : public reflect_base
		{
		public:
			reflect()
				: reflect_base(sizeof(*this), typeid(ValueType))
			{
				init();
			}

			reflect(const std::string& tags)
				: reflect_base(sizeof(*this), typeid(ValueType), tags)
			{
				init();
			}

			~reflect(){}
		public:
			reflect_base& Swap(reflect_base& r)
			{
				reflect<ValueType>* instance = dynamic_cast<reflect<ValueType>*>(&r);
				if (instance != nullptr)
				{
					std::swap(v_, instance->v_);
					this->reflect_base::Swap(r);
				}

				return *this;
			};

			reflect& operator=(const reflect& r)
			{
				reflect(r).Swap(*this);
				return *this;
			}

			reflect& operator=(reflect&& r)
			{
				r.Swap(*this);
				return *this;
			}

			template<class ValueType>
			reflect& operator=(ValueType&& v)
			{
				v_ = v;

				return *this;
			}

			const ValueType& operator()() const
			{
				return v_;
			}

			ValueType& operator()()
			{
				return v_;
			}
		public:
			virtual const boost::property_tree::ptree& Layout() const override { return layout_; }
			virtual const void* ValueAddr() const override { return &v_; }
			virtual bool IsArray() const override { return false; }
			virtual void ResizeArray(size_t count) override {/*do nothing*/ }
		private:
			// ������Ϣ
			struct ArrayInfo
			{
				std::string firstTag;
				boost::property_tree::ptree layout;
			};

			void init()
			{
				// buildLayout
				if (std::is_class<ValueType>::value && !std::is_same<ValueType, std::string>::value)
				{
#ifdef DAXIA_REFLECT_THREAD_SAFE
					layoutLocker_.lock();
#endif // DAXIA_REFLECT_THREAD_SAFE

					if (layout_.empty())
					{
						const char* start = reinterpret_cast<const char*>(&this->v_);
						const char* end = reinterpret_cast<const char*>(&this->v_) + sizeof(ValueType);
						buildLayout(start, start, end, layout_, nullptr);
					}

#ifdef DAXIA_REFLECT_THREAD_SAFE
					layoutLocker_.unlock();
#endif // DAXIA_REFLECT_THREAD_SAFE
				}
			}

			void buildLayout(const char* baseaddr,
				const char* start,
				const char* end,
				boost::property_tree::ptree& rootLayout,
				ArrayInfo* parentArray) const
			{
				try
				{
					for (; start < end; ++start)
					{
						const reflect_base* reflectBase = nullptr;

						try{ reflectBase = dynamic_cast<const reflect_base*>(reinterpret_cast<const reflect_helper*>(start)); }
						catch (const std::exception&){}

						if (reflectBase == nullptr) continue;

						boost::property_tree::ptree childLayout;
						childLayout.put(HASH, reflectBase->Type().hash_code());
						childLayout.put(OFFSET, reinterpret_cast<unsigned long>(start)-reinterpret_cast<unsigned long>(baseaddr));
						rootLayout.put_child(reflectBase->Tags(), childLayout);

						start += reflectBase->Size() - 1;
					}
				}
				catch (boost::property_tree::ptree_error)
				{

				}
			}

		private:
			ValueType v_;
			static boost::property_tree::ptree layout_;
			static std::mutex layoutLocker_;
		};// class reflect

		template<class ValueType>
		boost::property_tree::ptree reflect<ValueType>::layout_;
		template<class ValueType>
		std::mutex reflect<ValueType>::layoutLocker_;

		//////////////////////////////////////////////////////////////////////////
		// ���std::vector<ValueType>�����ػ�
		template<class ValueType>
		class reflect<std::vector<ValueType>> : public reflect_base
		{
		public:
			reflect()
				: reflect_base(sizeof(*this), typeid(std::vector<ValueType>))
			{
				init();
			}

			reflect(const std::string& tags)
				: reflect_base(sizeof(*this), typeid(std::vector<ValueType>), tags)
			{
				init();
			}

			~reflect(){}
		public:
			reflect_base& Swap(reflect_base& r)
			{
				reflect<ValueType>* instance = dynamic_cast<reflect<ValueType>*>(&r);
				if (instance != nullptr)
				{
					std::swap(v_, instance->v_);
					this->reflect_base::Swap(r);
				}

				return *this;
			};

			reflect& operator=(const reflect& r)
			{
				reflect(r).Swap(*this);
				return *this;
			}

			reflect& operator=(reflect&& r)
			{
				r.Swap(*this);
				return *this;
			}

			template<class ValueType>
			reflect& operator=(std::vector<ValueType>&& v)
			{
				v_ = v;

				return *this;
			}

			const std::vector<ValueType>& operator()() const
			{
				return v_;
			}

			std::vector<ValueType>& operator()()
			{
				return v_;
			}
		public:
			virtual const boost::property_tree::ptree& Layout() const override { return layout_; }
			virtual const void* ValueAddr() const override { return &v_; }
			virtual bool IsArray() const override { return true; }
			virtual void ResizeArray(size_t count) override
			{
				ValueType tempValue;
				std::vector<ValueType> temp(count, tempValue);
				std::swap(temp, v_);
			}
		private:
			// ������Ϣ
			struct ArrayInfo
			{
				std::string firstTag;
				boost::property_tree::ptree layout;
			};

			void init()
			{
				// buildLayout
				if (std::is_class<ValueType>::value && !std::is_same<ValueType, std::string>::value)
				{
					if (threadSafe_) layoutLocker_.lock();

					if (layout_.empty())
					{
						// vector���ͽ�������Ԫ�صĲ���
						layout_ = reflect<ValueType>().Layout();

						// ÿ��Ԫ�صĴ�С
						if (!layout_.empty()) layout_.put(SIZE, sizeof(ValueType));
					}

					if (threadSafe_) layoutLocker_.unlock();
				}
			}

		private:
			std::vector<ValueType> v_;
			static boost::property_tree::ptree layout_;
			static std::mutex layoutLocker_;
		};

		template<class ValueType>
		boost::property_tree::ptree reflect<std::vector<ValueType>>::layout_;
		template<class ValueType>
		std::mutex reflect<std::vector<ValueType>>::layoutLocker_;

	}// namespace reflect
}// namespace daxia

#undef HASH
#undef OFFSET
#undef SIZE

#endif // !__DAXIA_REFLECT_REFLECT_HPP

