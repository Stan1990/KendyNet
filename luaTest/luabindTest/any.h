/*	
    Copyright (C) <2012>  <huangweilook@21cn.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/	
#ifndef _ANY_H
#define _ANY_H


#include <vector>
#include "TypeList.h"
#include "Trait.h"

typedef LOKI_TYPELIST_11(char,unsigned char,short,unsigned short,int,unsigned int,long,unsigned long,float,double,void) internalType;

class luaObject;
class any;


typedef std::vector<any> luatable;

typedef LOKI_TYPELIST_13(char,unsigned char,short,unsigned short,int,unsigned int,long,unsigned long,float,double,std::string,luaObject,luatable) SupportType;

//ע�ᵽlua�е��û�����������Ϣ
template<typename T>
class luaRegisterClass
{
public:
	static bool isRegister()
	{
		return m_isRegister;
	}

	static void SetRegister()
	{
		m_isRegister = true;
	}

	static void ClearRegister()
	{	
		m_isRegister = false;
	}

	static void SetClassName(const char *name)
	{
		classname = name;
	}

	static const char *GetClassName()
	{
		return classname.c_str();
	}

private:
	static bool m_isRegister;
	static std::string classname;
};

template<typename T>
bool luaRegisterClass<T>::m_isRegister = false;

template<typename T>
std::string luaRegisterClass<T>::classname;

//һ��������boost::any
class any
{
public: // structors

   any(): content(0),counter(0)
   {

   }

   template<typename ValueType>
   any(const ValueType & value)
      : content(new holder<ValueType>(value)),counter(new int(1)),luaRegisterClassName("")
   {
	   //printf("create\n");
	   m_type = IndexOf<SupportType,ValueType>::value;
	   isPointerType = pointerTraits<ValueType>::isPointer;
	   luaRegisterClassName = 	luaRegisterClass<pointerTraits<ValueType>::PointeeType>::GetClassName();
   }

   any(const any & other)
   {
	   if(other.counter && other.content)
	   {
		   content = other.content;
		   counter = other.counter;
		   ++(*counter);
		   m_type = other.m_type;
		   isPointerType = other.isPointerType;
		   luaRegisterClassName = other.luaRegisterClassName;
	   }
	   else
	   {
		   content = 0;
		   counter = 0;
		   m_type = 0;
	   }
   }

   	any & operator=(const any & rhs)
	{
		if(&rhs == this)
			return *this;
		else
		{      
			_destroy();
			if(rhs.counter && rhs.content)
			{
				content = rhs.content;
				counter = rhs.counter;
				++(*counter);
				m_type = rhs.m_type;
				isPointerType = rhs.isPointerType;
			}
			else
			{
				content = 0;
				counter = 0;
				m_type = 0;
			}
			return *this;
		}
	}

   ~any()
   {
		_destroy();
   }

   short GetType() const
   {
	   return m_type;
   }

   bool IsPointerType() const
   {
	   return isPointerType;
   }

private:
	void _destroy()
	{
		if(counter && content && --*counter <= 0)
		{
			delete counter;
			delete content;
			//printf("destroy\n");
		}
	}

	short m_type;
	bool  isPointerType;

public: // queries

    bool empty() const
    {
       return !content;
    }

public:

    class placeholder
    {
      public: // structors

		virtual ~placeholder()
        {
        }

     };

     template<typename ValueType>
     class holder : public placeholder
     {
       public: // structors

         holder(const ValueType & value)
           : held(value)
         {
         }
       public: // representation

          ValueType held;

      };

public: // representation (public so any_cast can be non-friend)

   placeholder * content;
   int         * counter;
   std::string luaRegisterClassName;//�����������lua��ע�������

};

template<typename ValueType>
inline ValueType any_cast(const any & operand)
{
	any::holder<ValueType> * tmp = static_cast<any::holder<ValueType> *>(operand.content);
	return tmp->held;
}

template<>
inline std::string any_cast(const any & operand)
{
	if(operand.GetType() < 10)
	{
		//��ֵ��,����ת��string
		char tmp[32];
		unsigned long value = any_cast<unsigned long>(operand);
		ltoa(value,tmp,10);
		return std::string(tmp);

	}
	if(operand.GetType() == 10)
	{
		any::holder<std::string> *tmp = static_cast<any::holder<std::string> *>(operand.content);
		return tmp->held;
	}
	return std::string("");
}


//��Ŵ�lua�з��ص�һ��ֵ,volume��ʾ����ֵ������
template<int volume>
class lua_results
{
public:

	lua_results():counter(new int(1)),rets(new std::vector<any>),_rets(*rets)
	{
	}

	~lua_results()
	{
		if(counter)
		{
			if(--*counter <= 0)
			{
				delete counter;
				delete rets;
				printf("retarray destroy\n");
			}
		}
	}

	lua_results(const lua_results & other):counter(other.counter),rets(other.rets),_rets(*rets)
	{
		if(counter)
			++(*counter);
	}
	
	

private:
	int * counter;
	std::vector<any> *rets;
public:
	std::vector<any> &_rets;

private:
	lua_results<volume> & operator=(const lua_results<volume> & rhs);
};




#endif