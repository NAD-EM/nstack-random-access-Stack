/**
	nstack: c++ library for implementing random access stacks.
    Copyright (C) 2017  NAD-EM

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

    You can contact the author through email:
	<git.nad.em.00@gmail.com>.
	Or through their GitHub profile:
	<https://github.com/NAD-EM>.
*/

#ifndef NSTACK_HPP_
#define NSTACK_HPP_

template <typename S>
class Stack
{
private:

	template <typename T, typename R, typename Q>
	class BaseStack
	{
	public:

		//return 0 on succes, 1 if stack has reached user-defined max size, 2 if memory allocation failed.
		virtual int Push(const T element, Q* Package) = 0;//
		virtual int InsertAsLastElement(const T element, Q* Package) = 0;//
		virtual int InsertAtIndex(const T element, const unsigned int index, Q* Package) = 0;//from 0 to n.

	};

	template <typename T, typename R, typename Q>
	class LimitlessStack : private BaseStack<T, R, Q>
	{
	private:
		int Push(const T element, Q* Package);//
		int InsertAsLastElement(const T element, Q* Package);//
		int InsertAtIndex(const T element, const unsigned int index, Q* Package);
	};

	template <typename T, typename R, typename Q>
	class LimitedStack : private BaseStack<T, R, Q>
	{
	private:
		int Push(const T element, Q* Package);//
		int InsertAsLastElement(const T element, Q* Package);//
		int InsertAtIndex(const T element, const unsigned int index, Q* Package);
	};

	struct Handle
	{
		Stack<S> *BaseObject;
		struct Handle *BaseHandlePtr;
		int IndexOne;
		int IndexTwo;
		unsigned int temp;

		void ClearIndexes(void)
		{
			BaseHandlePtr->IndexOne = -1;
			BaseHandlePtr->IndexTwo = -1;
		}

		//default constructor:
		Handle() : BaseObject(0), BaseHandlePtr(0), IndexOne(-1), IndexTwo(-1) {}

		//Constructor:
		//pass entrance arguments of call to the
		//set of variables we have in the base class scope
		Handle(Stack<S> *baseObject, Handle *instance, unsigned int index) : BaseObject(baseObject), BaseHandlePtr(instance), temp(index)
		{
			if (BaseHandlePtr->IndexOne == -1) BaseHandlePtr->IndexOne = (signed int)temp;
			else if (BaseHandlePtr->IndexOne != -1) BaseHandlePtr->IndexTwo = (signed int)temp;
		}

		//push value at index as a new instance (this does not swap indexes).
		//returns 1 if stack has reched user defined max size, 2 if memory could not be allocated,
		//3 if index one is out of range, 4 if index two is out of range, 0 on success.
		int operator << (Handle RightHandOperand)
		{
			unsigned int indexOne = (unsigned)BaseHandlePtr->IndexOne;
			unsigned int indexTwo = (unsigned)BaseHandlePtr->IndexTwo;
			ClearIndexes();

			if (indexTwo >= BaseObject->Package.size) return 3;
			if (indexOne >= BaseObject->Package.size) return 4;

			element *RightHand = BaseObject->GetPointerToElementBehindIndex(indexOne);
			return BaseObject->InsertAtIndex(RightHand->Next->Contents, indexTwo);
		}

		//Retrieve content at begining of stack by value.
		//returns 3 if index is out of range.
		int operator >> (S *dest)
		{
			unsigned int indexOne = (unsigned)BaseHandlePtr->IndexOne;
			ClearIndexes();

			if (indexOne >= BaseObject->Package.size) return 3;
			*(dest) = BaseObject->RetrieveByValueAtIndex(indexOne);
			return 0;
		}

		//Swap indexes.
		//returns 1 if index one is out of range, 2 if index two is out of
		//range, 3 if indexes are equal, 0 on success
		int operator <<= (Handle )
		{
			unsigned int indexOne = (unsigned)BaseHandlePtr->IndexOne;
			unsigned int indexTwo = (unsigned)BaseHandlePtr->IndexTwo;
			ClearIndexes();

			return BaseObject->SwapTwoIndexes(indexTwo, indexOne);
		}

		//pop at index
		//returns 3 if index one is out of range, 0 on success.
		int operator --()
		{
			ClearIndexes();
			if (temp >= BaseObject->Package.size)
				return 3;
			return BaseObject->PopElementAtIndex(temp);
		}

		//copy by value element in one index into another index (this does not swap indexes).
		//returns 3 if index one is out of range, 4 if index two is out of range, 0 on success.
		int operator = (Handle RightHandOperand)//copy from one index to another index
		{
			unsigned int indexOne = (unsigned)BaseHandlePtr->IndexOne;
			unsigned int indexTwo = (unsigned)BaseHandlePtr->IndexTwo;
			ClearIndexes();

			if (indexTwo >= BaseObject->Package.size) return 3;
			if (indexOne >= BaseObject->Package.size) return 4;

			element *RightHand = BaseObject->GetPointerToElementBehindIndex(indexOne);
			element *LeftHand = BaseObject->GetPointerToElementBehindIndex(indexTwo);
			LeftHand->Next->Contents = RightHand->Next->Contents;

			return 0;
		}

		//returns false if different or if indexes are out of range, true otherwise
		bool operator == (Handle RightHandOperand)
		{
			unsigned int indexOne = (unsigned)BaseHandlePtr->IndexOne;
			unsigned int indexTwo = (unsigned)BaseHandlePtr->IndexTwo;
			ClearIndexes();

			if (indexTwo >= BaseObject->Package.size) return false;
			if (indexOne >= BaseObject->Package.size) return false;

			element *RightHand = BaseObject->GetPointerToElementBehindIndex(indexOne);
			element *LeftHand = BaseObject->GetPointerToElementBehindIndex(indexTwo);

			if (LeftHand->Next->Contents == RightHand->Next->Contents)
				return true;
			return false;
		}
	};
	Handle *Instance;

	struct element
	{
		S Contents;
		element *Next;
	};

	struct package
	{
		unsigned size;
		int maxSize;
		element *First;
	} Package;

	BaseStack<S, element, package> *MethodCallingPtr;

protected:

	//from 0 to 'n-1', where n is the number of elements in stack
	inline element *GetPointerToElementBehindIndex(unsigned int index)
	{
		element *Ptr = Package.First;
		++index;
		while (--index)
			Ptr = Ptr->Next;
		return Ptr;
	}

public:
	void FreeStack(void);//eliminates all of the stack contents.
	void SetMaxSize(const long MaxSize);//negative numbers disable max size protection. This method resizes the stack if current size is bigger than user-set limit, may cause lose of data.
	bool StackHasMaxSizeSet(void) { return (Package.maxSize == -1 ? false : true); }//
	bool IsEmpty(void) { return Package.size == 0 ? true : false; }//
	int  StackSize(void) { return Package.size; }//
	int  StackSizeInBytes(void) { return (Package.size*sizeof(element)); }//returns a shallow aproximate

	//return 0 on succes, 1 if stack has reached user-defined max size.
	int Push(const S element);
	int InsertAsLastElement(const S element);
	int InsertAtIndex(const S element, const unsigned int index);

	//return pointer to popped element. returns 0 on failure
	void Pop(void);//
	void PopLastElement(void);//
	int  PopElementAtIndex(const unsigned int index);//returns 1 if index is out of range

	S RetrieveByValueFirstElement(void);
	S RetrieveByValueLastElement(void);
	S RetrieveByValueAtIndex(const unsigned int index);

	S & RetrieveByReferenceFirstElement(void);
	S & RetrieveByReferenceLastElement(void);
	S & RetrieveByReferenceAtIndex(const unsigned int index);

	//the indexes go from 0 to 'n-1' where 'n' is the number of elements in the stack.
	//return 1 if first index is out of range, 2 if second index is out of range, 3 if they are equal, 0 on success
	int SwapTwoIndexes(const unsigned Index_One, const unsigned Index_Two);
	int SwapFirstElementWithIndex(const unsigned Index);
	int SwapLastElementWithIndex(const unsigned Index);
	int SwapFirstAndLastIndexes(void);

	void InvertStack(void);//

	//handle indexes
	Handle operator [] (unsigned int index)
	{
		return Handle(this, Instance, index);
	}

	//short for push
	int operator << (S element)
	{
		return Push(element);
	}

	//Retrieve content at begining of stack by value
	int operator >> (S *dest)
	{
		*(dest) = RetrieveByValueFirstElement();
		return 0;
	}

	//short for pop
	void operator --()
	{
		Pop();
	}

	Stack();
	~Stack();
};


//////
//		Base methods
//////
template <typename S>
void Stack<S>::FreeStack(void)
{
	unsigned int Size = Package.size;
	++Size;
	while(--Size)
		this->Pop();

	Package.size = 0;
}

template <typename S>
void Stack<S>::SetMaxSize(const long MaxSize)
{
	delete MethodCallingPtr;
	if (MaxSize >= 0)
	{
		MethodCallingPtr = (BaseStack<S, element, package>*)(new LimitlessStack<S, element, package>);
		Package.maxSize = MaxSize;
	}
	else
	{
		MethodCallingPtr = (BaseStack<S, element, package>*)(new LimitedStack<S, element, package>);
		Package.maxSize = -1;
	}
}
//////

//////
//		Control methods
/////
template <typename S>
int Stack<S>::Push(const S element)
{
	return MethodCallingPtr->Push(element, &Package);
}

template <typename S>
int Stack<S>::InsertAsLastElement(const S element)
{
	return MethodCallingPtr->InsertAsLastElement(element, &Package);
}

template <typename S>
int Stack<S>::InsertAtIndex(const S element, const unsigned int index)
{
	return MethodCallingPtr->InsertAtIndex(element, index, &Package);
}

template <typename S>
void Stack<S>::Pop(void)
{
	if (Package.size > 0)
	{
		element *AuxPtr = Package.First->Next;
		Package.First->Next = Package.First->Next->Next;
		delete (AuxPtr);

		Package.size--;
	}
}

template <typename S>
void Stack<S>::PopLastElement(void)
{
	element *FirstPosition = Package.First;
	Package.First = GetPointerToElementBehindIndex(Package.size - 1);
	this->Pop();
	Package.First = FirstPosition;
}

template <typename S>
int Stack<S>::PopElementAtIndex(const unsigned int index)
{
	int returnVal = 1;
	if (index < Package.size)
	{
		unsigned int Index = index;
		element *FirstPosition = Package.First;
		Package.First = GetPointerToElementBehindIndex(Index);
		this->Pop();
		Package.First = FirstPosition;

		returnVal = 0;
	}
	return returnVal;
}

template <typename S>
int Stack<S>::SwapTwoIndexes(const unsigned Index_One, const unsigned Index_Two)
{
	unsigned int Size = Package.size;
	if (Index_One >= Size) return 1;
	if (Index_Two >= Size) return 2;
	if (Index_One == Index_Two)    return 3;

	element *FirstIndex = Package.First;
	element *SecondIndex;
	unsigned IndexOne = Index_One;
	unsigned IndexTwo = Index_Two;
	S temp;

	if (Index_One > Index_Two)
	{
		unsigned temp = IndexTwo;
		IndexTwo = IndexOne;
		IndexOne = temp;
	}
	++IndexOne;
	++IndexTwo;

	if (Index_One != 0)
	{
		IndexOne -= 1;
		IndexTwo -= IndexOne;
		FirstIndex = FirstIndex->Next;
	}

	//navigate to positions prior to the nodes to modify
	while (--IndexOne)
		FirstIndex = FirstIndex->Next;

	SecondIndex = FirstIndex;
	while (--IndexTwo)
		SecondIndex = SecondIndex->Next;

	//make a shallow copy
	temp = FirstIndex->Next->Contents;
	FirstIndex->Next->Contents = SecondIndex->Next->Contents;
	SecondIndex->Next->Contents = temp;

	return 0;
}

template <typename S>
int Stack<S>::SwapFirstElementWithIndex(const unsigned Index)
{
	return SwapTwoIndexes(0, Index);
}

template <typename S>
int Stack<S>::SwapLastElementWithIndex(const unsigned Index)
{
	return SwapTwoIndexes(Index, Package.size - 1);
}

template <typename S>
int Stack<S>::SwapFirstAndLastIndexes(void)
{
	return SwapTwoIndexes(0, Package.size - 1);
}

template <typename S>
void Stack<S>::InvertStack(void)
{
	int position = Package.size;
	if (position > 0)
	{
		element *NewStart = new (element);
		element *Aux = Package.First->Next;

		//Acomodate new end of stack
		NewStart->Next = Aux;
		Package.First->Next = Aux->Next;
		Aux->Next = 0;

		//reacomodate contents in new root
		while (--position)
		{
			Aux = Package.First->Next;
			Package.First->Next = Aux->Next;
			Aux->Next = NewStart->Next;
			NewStart->Next = Aux;
		}

		delete Package.First;
		Package.First = NewStart;
	}
}

template<typename S>
S Stack<S>::RetrieveByValueAtIndex(const unsigned int index)
{
	S returnVal = S();
	if (index < Package.size)
	{
		element *FirstElement = Package.First;
		Package.First = GetPointerToElementBehindIndex(index);
		returnVal = Package.First->Next->Contents;
		Package.First = FirstElement;
	}
	return returnVal;
}

template<typename S>
S Stack<S>::RetrieveByValueFirstElement(void)
{
	S returnVal = Package.First->Next->Contents;
	return returnVal;
}

template<typename S>
S Stack<S>::RetrieveByValueLastElement(void)
{
	S returnVal = S();

	element *FirstElement = Package.First;
	Package.First = GetPointerToElementBehindIndex(Package.size - 1);
	returnVal = Package.First->Next->Contents;
	Package.First = FirstElement;

	return returnVal;
}

template<typename S>
S & Stack<S>::RetrieveByReferenceAtIndex(const unsigned int index)
{
	S returnVal = S();
	if (index < Package.size)
	{
		element *Element = GetPointerToElementBehindIndex(Package.size - 1);
		return Element->Contents;
	}
	return returnVal;
}

template<typename S>
S & Stack<S>::RetrieveByReferenceFirstElement(void)
{
	return Package.First->Next->Contents;
}

template<typename S>
S & Stack<S>::RetrieveByReferenceLastElement(void)
{
	element *Element = GetPointerToElementBehindIndex(Package.size - 1);
	return Element->Contents;
}
//////

//////
//		Limited Stack
//////
template <typename S>
template <typename T, typename R, typename Q>
int Stack<S>::LimitedStack<T, R, Q>::Push(const T element, Q* Package)
{
	if (Package.size == Package.size) return 1;

	R *NewElement = new (R);
	if (NewElement == 0) return 2;
	NewElement->Contents = element;

	NewElement->Next = Package->First->Next;
	Package->First->Next = NewElement;

	Package->size++;
	return 0;
}

template <typename S>
template <typename T, typename R, typename Q>
int Stack<S>::LimitedStack<T, R, Q>::InsertAsLastElement(const T element, Q* Package)
{
	if (Package.size == Package.size) return 1;
	int ReturnVal = 1;
	R *FirstPosition = Package->First;

	for (int i = 0; i < Package->size; i++)
		Package->First = Package->First->Next;
	ReturnVal = this->Push(element, Package);
	Package->First = FirstPosition;

	return ReturnVal;
}

template <typename S>
template <typename T, typename R, typename Q>
int Stack<S>::LimitedStack<T, R, Q>::InsertAtIndex(const T element, const unsigned int index, Q* Package)
{
	if (index >= Package.size) return 3;
	if (Package.size == Package.size) return 1;
	int ReturnVal = 1;
	R *FirstPosition = Package->First;

	for (int i = 0; i<index; i++)
		Package->First = Package->First->Next;
	ReturnVal = this->Push(element, Package);
	Package->First = FirstPosition;

	return ReturnVal;
}
//////

//////
//		Limitless Stack
//////
template <typename S>
template <typename T, typename R, typename Q>
int Stack<S>::LimitlessStack<T, R, Q>::Push(const T element, Q* Package)
{
	R *NewElement = new (R);
	if (NewElement == 0) return 2;
	NewElement->Contents = element;

	NewElement->Next = Package->First->Next;
	Package->First->Next = NewElement;

	Package->size++;
	return 0;
}

template <typename S>
template <typename T, typename R, typename Q>
int Stack<S>::LimitlessStack<T, R, Q>::InsertAsLastElement(const T element, Q* Package)
{
	int ReturnVal = 1;
	R *FirstPosition = Package->First;

	for (unsigned i = 0; i<Package->size; i++)
		Package->First = Package->First->Next;
	ReturnVal = this->Push(element, Package);
	Package->First = FirstPosition;

	return ReturnVal;
}

template <typename S>
template <typename T, typename R, typename Q>
int Stack<S>::LimitlessStack<T, R, Q>::InsertAtIndex(const T element, const unsigned int index, Q* Package)
{
	if (index >= Package->size) return 3;
	int ReturnVal = 1;
	R *FirstPosition = Package->First;

	for (unsigned i = 0; i<index; i++)
		Package->First = Package->First->Next;
	ReturnVal = this->Push(element, Package);
	Package->First = FirstPosition;

	return ReturnVal;
}
//////

template <typename S>
Stack<S>::Stack()
{
	Package.maxSize = -1;
	Package.size = 0;
	Package.First = new (element);
	Package.First->Next = 0;

	Instance = new Handle();

	MethodCallingPtr = (BaseStack<S, element, package>*)(new LimitlessStack<S, element, package>());
}

template <typename S>
Stack<S>::~Stack()
{
	this->FreeStack();
	delete MethodCallingPtr;
	delete Package.First;
	delete Instance;
}

#endif  // NSTACK_HPP_
