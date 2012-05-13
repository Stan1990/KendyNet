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
#ifndef _OBJPUSH_H
#define _OBJPUSH_H
//����lua����ʱ����ѹջ�ĳ���
template<typename T>
class objPush
{
public:
	objPush(lua_State *L,const T arg)
	{
		doPush(L,arg,Int2Type<pointerTraits<T>::isPointer>(),Int2Type<IndexOf<internalType,pointerTraits<T>::PointeeType>::value == -1>());
	}
private:
	 //����ָ������ָ��
	 void doPush(lua_State *L,T arg,Int2Type<true>,Int2Type<true>)
	 {
		if(!arg)
			lua_pushnil(L);
		else
		{
			if(!luaRegisterClass<pointerTraits<T>::PointeeType>::isRegister())
				lua_pushlightuserdata(L,(void*)arg);
			else
				objUserData<pointerTraits<T>::PointeeType>::NewObj(L,arg);
		}
	}

	//����ָ���ڲ����͵�ָ��
	void doPush(lua_State *L,T arg,Int2Type<true>,Int2Type<false>)
	{
		if(!arg)
			lua_pushnil(L);
		{
			typedef LOKI_TYPELIST_2(char,unsigned char) CharType;
			//ָ���ڲ����͵�ָ���п�����char *
			doPushUserData(L,arg,Int2Type<IndexOf<CharType,pointerTraits<T>::PointeeType>::value != -1>());
		}
	}

	//����ָ���ڲ�����
	void doPush(lua_State *L,T arg,Int2Type<false>,Int2Type<false>)
	{
		lua_pushnumber(L,(lua_Number)arg);
	}
	
	//���ڴ���ָ��char *��ָ��
	void doPushUserData(lua_State *L,T arg, Int2Type<true>)
	{
		lua_pushstring(L,arg);
	}

	//��������ָ���ڲ����͵�ָ��
	void doPushUserData(lua_State *L,T arg, Int2Type<false>)
	{
		lua_pushlightuserdata(L,(void*)arg);
	}

};

//��std::string���ػ�
template<>
class objPush<std::string>
{
public:
	objPush(lua_State *L,const std::string &arg)
	{
		lua_pushstring(L,arg.c_str());
	}
};

//���luaObject���ػ�
template<>
class objPush<luaObject>
{
public:
	objPush(lua_State *L,const luaObject arg)
	{
		lua_rawgeti(L,LUA_REGISTRYINDEX,arg.getIndex());
	}
};


//���luatable���ػ�
template<>
class objPush<luatable>
{
public:
	objPush(lua_State *L,const luatable &arg)
	{
		lua_newtable(L);
		for(int i = 0; i < (int)arg.size(); ++i)
		{
			//lua_pushnumber(L,i+1);
			
			if(arg[0].empty())
			{
				lua_pushnil(L);
			}
			else
			{	

				short t_type = arg[i].GetType();
				if(t_type == -1)
				{
					if(arg[i].IsPointerType())
					{
						//�Ƿ�ע����û�����
						if(arg[i].luaRegisterClassName == "")
							objPush<void*> obj(L,any_cast<void*>(arg[i]));
						else
						{
							NewObj(L,any_cast<void*>(arg[i]),arg[i].luaRegisterClassName.c_str());
						}
					}
					else
					{
						//����
					}
				}
				else if(t_type >=0 && t_type <= 9)
				{
					switch(t_type)
					{
					case 0:
					case 1:
						lua_pushnumber(L,any_cast<unsigned char>(arg[i]));
						break;
					case 2:
					case 3:
						lua_pushnumber(L,any_cast<unsigned short>(arg[i]));
						break;
					case 4:
					case 5:
						lua_pushnumber(L,any_cast<unsigned int>(arg[i]));
						break;
					case 6:
					case 7:
						lua_pushnumber(L,any_cast<unsigned long>(arg[i]));
						break;
					case 8:
						lua_pushnumber(L,any_cast<float>(arg[i]));
						break;
					case 9:
						lua_pushnumber(L,any_cast<double>(arg[i]));
						break;
					};
				}
				else if(t_type == 10)
				{
					lua_pushstring(L,any_cast<std::string>(arg[i]).c_str());
				}
				else if(t_type == 11)
				{
					objPush<luaObject> obj(L,any_cast<luaObject>(arg[i]));
				}
				else if(t_type == 12)
				{
					objPush<luatable> obj(L,any_cast<luatable>(arg[i]));
				}
			}

			lua_rawseti(L,-2,i+1);
		}
	}
};


#endif