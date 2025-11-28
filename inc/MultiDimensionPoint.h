#pragma once
template<typename T, int N>
struct MultiDimensionPoint
{
    typedef MultiDimensionPoint<T,N> _self_type;

    _self_type ()
    {
        for (int i = 0; i < N; i++)
        {
            d[i] = T(0);
        }
    }

     _self_type operator -(const _self_type& right) const
    {
        _self_type tmp;
        for(int i=0; i<N; i++)
        {
            tmp.d[i] = d[i] - right.d[i];
        }        
        return tmp;
    }

     _self_type operator +(const _self_type& right) const
    {
        _self_type tmp;
        for(int i=0; i<N; i++)
        {
            tmp.d[i] = d[i] + right.d[i];
        }        
        return tmp;
    }


     _self_type& operator +=(const _self_type& right)
    {
        for(int i=0; i<N; i++)
        {
            d[i] += right.d[i];
        }        
        return *this;
    }

     _self_type& operator -=(const _self_type& right)
    {
        for(int i=0; i<N; i++)
        {
            d[i] -= right.d[i];
        }        
        return *this;
    }

     _self_type& operator *=(const _self_type& right)
    {
        for(int i=0; i<N; i++)
        {
            d[i] *= right.d[i];
        }        
        return *this;

    }

     _self_type& operator /=(const _self_type& right)
    {
        for(int i=0; i<N; i++)
        {
            d[i] /= right.d[i];
        }        
        return *this;

    }



    _self_type& operator /=(T value)
    {
        for(int i=0; i<N; i++)
        {
            d[i] /= value;
        }
        
        return *this;
    }


    _self_type& operator *=(T value)
    {
        for(int i=0; i<N; i++)
        {
            d[i] *= value;
        }
        
        return *this;
    }
  
    bool operator !=(const _self_type& right)
    {
        for(int i=0; i<N; i++)
        {
            if(d[i] != right.d[i])
            {
                return true;
            }
        }

        return false;
        
    }


    bool operator ==(const _self_type& right)
    {
        for(int i=0; i<N; i++)
        {
            if(d[i] != right.d[i])
            {
                return false;
            }
        }

        return true;
        
    }

    T& operator[](int i)
    {
        assert(i < N && i>=0);
        return d[i];
    }

    const T& operator[](int i)const 
    {
        assert(i < N && i>=0);
        return d[i];
    }


    T d[N];
};



template<class T, int N>
inline  MultiDimensionPoint<typename T, N> operator *(const MultiDimensionPoint<typename T, N>& v, const T& scalar)
{
   MultiDimensionPoint<typename T, N> newVector;

    for(int i=0; i<N; i++)
    {
        newVector.d[i] = v.d[i]*scalar;
    }

    return newVector;
}


template<class T, int N>
inline  MultiDimensionPoint<typename T, N> operator *( const T& scalar, const MultiDimensionPoint<typename T, N>& v )
{
    return operator*(v, scalar);
}


template<class T, int N>
inline  MultiDimensionPoint<typename T, N> operator /(const MultiDimensionPoint<typename T, N>& v, const T& scalar)
{
   MultiDimensionPoint<typename T, N> newVector;

   if(scalar <= std::numeric_limits<T>::epsilon())
   {
       assert(false);
   }
    for(int i=0; i<N; i++)
    {
        newVector.d[i] = v.d[i]/scalar;
    }

    return newVector;
}





template<class T, int N>
inline T norm(const MultiDimensionPoint<typename T, N>& mdp)
{
    T sum = 0.0;
    for(int i=0; i<N; i++)
    {
        sum += mdp.d[i]*mdp.d[i];
    }

    return sqrt(sum);
}


//dot product of vector v1,v2
//dot(v1,v2)=v1'*v2;
template<class T, int N>
inline T dot(const MultiDimensionPoint<typename T, N>& v1, const MultiDimensionPoint<typename T, N>& v2)
{
    T dot_product = 0.0;

    for(int i=0; i<N; i++)
    {
        dot_product += v1.d[i]*v2.d[i];
    }

    return dot_product;
}


//cross product of vector v1,v2
//cross(v1,v2)=v1[0]*v2[1] - v1[1]*v2[0];
template<class T>
inline T cross(const MultiDimensionPoint<typename T, 2>& v1, const MultiDimensionPoint<typename T, 2>& v2)
{
    T cross_product = 0.0;

    cross_product = v1.d[0]*v2.d[1] - v1.d[1]*v2.d[0];

    return cross_product;
}


template<class T>
inline MultiDimensionPoint<typename T, 3> cross(const MultiDimensionPoint<typename T, 3>& v1, const MultiDimensionPoint<typename T, 3>& v2)
{
   MultiDimensionPoint<typename T, 3> cross_product;

    cross_product.d[0] =  v1.d[1]*v2.d[2] - v1.d[2]*v2.d[1];
    cross_product.d[1] =  v1.d[2]*v2.d[0] - v1.d[0]*v2.d[2];
    cross_product.d[2] =  v1.d[0]*v2.d[1] - v1.d[1]*v2.d[0];

    return cross_product;
}


typedef MultiDimensionPoint<double, 3> TPoint3D;
typedef MultiDimensionPoint<double, 3> TVector3D;
typedef MultiDimensionPoint<double, 2> TPoint2D;
typedef MultiDimensionPoint<double, 2> TVector2D;

