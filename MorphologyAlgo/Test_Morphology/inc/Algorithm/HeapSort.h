#pragma once
template<class T>
class CHeapSortAlgorithm
{
public:
	void operator()(T* pAryObj, UINT nSize, bool bAscend)
	{
		HeapSort(pAryObj, nSize, bAscend);

	}


protected:

	//@函数名:HeapSort
	//@功  能:堆排序法排列m_aryMotionObjects中的内容
	//@参  数:
	//        bAscend,是否以升序排列
	//@返回值:m_aryMotionObjects中的内容发生变化返回TRUE,否则返回FALSE;
	//@说  明:用堆排序法排序时的时间复杂度是nlog(2)N,比冒泡法排序方法进步很多
	bool HeapSort(T* pAryObj, UINT nSize, bool bAscend)
	{
		int k = 0;
		int n = nSize;//[0]~[n-1],下标为K的节点的左右孩子结点的下标分别是2*K+1和2*K+2,非叶
		//结点的下标最大值为(n-1)/2 -1,注意,实际上是一个完全二叉数
		bool bRes = false;
		//初始化堆
		for(k=(n-1)/2;k>=0;k--)
		{
			bRes |= HeapAdjust(pAryObj, k, n-1, bAscend);
		}

		for(k=n-1;k>0;k--)//k=0,不需要再排序了,因为[0]~[0]自然有序
		{
			//堆顶的元素是[0]~[k]中的极大值(或者极小值)
			T obj = pAryObj[k];
			pAryObj [k] = pAryObj[0];//[k],..,[n-1]已经有序
			pAryObj[0] = obj;
			bRes |= HeapAdjust(pAryObj, 0, k-1, bAscend);//调整[0]~[k-1]组成的堆,除[0]外,[1]~[k-1]均为大顶堆(小顶堆)

		}//for

		return bRes;
	}

	//@函数名:HeapAdjust
	//@功  能:堆排序法排列方法的调整操作
	//@参  数:r,起始下标;
	//        s,结束下标;
	//        bAscend,是否以升序排列;
	//@返回值:堆中元素位置发生变化,返回TRUE;
	//@说  明:除了[r]元素外,[r+1],...,[s]已经是大顶堆(bAscend=TRUE)或小顶堆(bAscend=FALSE)

	bool HeapAdjust(T* pAryObj,int r,int s,bool bAscend)
	{
		int rOld = r;
		T obj = pAryObj[r];
		int k=0;
		for(k=2*r+1;k<=s;k=2*k+1)
		{
			//如果是升序排列,即要调整为大顶堆,
			if(bAscend)
			{
				if(k<s &&  pAryObj[k+1] > pAryObj[k] ) ++k;//k为值较大的元素的下标

				if(obj >= pAryObj[k]) break;
				pAryObj[r] = pAryObj[k];
				r = k;

			}
			else//如果是降序排列,即要调整为小顶堆,
			{
				if(k<s &&  pAryObj[k+1] < pAryObj[k]) ++k;//k为值较小的元素的下标
				if(obj <= pAryObj[k]) break;
				pAryObj[r] = pAryObj[k];
				r = k;
			}

		}
		pAryObj[r]= obj;
		return (rOld==r);
	}

public:

};