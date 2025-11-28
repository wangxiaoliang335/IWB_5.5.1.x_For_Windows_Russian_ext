#pragma once
//矢量类
template <class T, int Dimension>
class Vec
{
public:
	Vec& operator=(const Vec& r)
	{
		for(int i=0; i<Dimension; i++)
		{
			m_data[i] = r.m_data[i];
		}
		return *this;
	}
	T& operator[](int index)
	{
		return m_data[index];
	}

	const T& operator[](int index) const
	{
		return m_data[index];
	}

	Vec& operator- (const Vec&r)
	{
		for(int i=0; i<Dimension; i++)
		{
			m_data[i] -= r.m_data[i];
		}
		return *this;
	}

	Vec& operator-= (const Vec&r)
	{
		for(int i=0; i<Dimension; i++)
		{
			m_data[i] -= r.m_data[i];
		}
		return *this;
	}
	

	Vec& operator+ (const Vec&r)
	{
		for(int i=0; i<Dimension; i++)
		{
			m_data[i] += r.m_data[i];
		}
		return *this;
	}

	Vec& operator+= (const Vec&r)
	{
		for(int i=0; i<Dimension; i++)
		{
			m_data[i] += r.m_data[i];
		}
		return *this;
	}


	//@功能:将矢量内容保存在ASCII文件中
	//参 数:szFileName, 文件名
	//@说明:每列数据之间以两个空格分隔开,和Matlab中保存的格式一致
	bool Save(const char* szFileName)const
	{
		FILE* file = NULL;
			
		errno_t err =  fopen_s(&file, szFileName, "wt");

		if(err == EINVAL)
		{
			return false;
		}

		for(int c=0; c < Dimension; c++)
		{
			fprintf(file, "  %.7e", m_data[c]);

		}//for-each(col)

		fprintf(file, "\r\n");		

		fclose(file);
		return true;
	}


protected:
	T m_data[Dimension];
};