#pragma once

#include "dt_common.h"

#ifdef GetProp
#undef GetProp
#endif

#define ADDRESSPROXY_NONE	-1

class RecvTable;
class RecvProp;

class CRecvProxyData
{
public:
	const RecvProp* m_pRecvProp;

	DVariant		m_Value;

	int				m_iElement;

	int				m_ObjectID;
};

typedef void (*RecvVarProxyFn)(const CRecvProxyData* pData, void* pStruct, void* pOut);

typedef void (*ArrayLengthRecvProxyFn)(void* pStruct, int objectID, int currentArrayLength);

typedef void (*DataTableRecvVarProxyFn)(const RecvProp* pProp, void** pOut, void* pData, int objectID);

class CRecvDecoder;
class RecvProp
{

public:
	RecvProp();

	void					InitArray(int nElements, int elementStride);

	int						GetNumElements() const;
	void					SetNumElements(int nElements);

	int						GetElementStride() const;
	void					SetElementStride(int stride);

	int						GetFlags() const;

	const char* GetName() const;
	SendPropType			GetType() const;

	RecvTable* GetDataTable() const;
	void					SetDataTable(RecvTable* pTable);

	RecvVarProxyFn			GetProxyFn() const;
	void					SetProxyFn(RecvVarProxyFn fn);

	DataTableRecvVarProxyFn	GetDataTableProxyFn() const;
	void					SetDataTableProxyFn(DataTableRecvVarProxyFn fn);

	int						GetOffset() const;
	void					SetOffset(int o);

RecvProp* GetArrayProp() const;
	void					SetArrayProp(RecvProp* pProp);

void					SetArrayLengthProxy(ArrayLengthRecvProxyFn proxy);
	ArrayLengthRecvProxyFn	GetArrayLengthProxy() const;

	bool					IsInsideArray() const;
	void					SetInsideArray();

const void* GetExtraData() const;
	void				SetExtraData(const void* pData);

const char* GetParentArrayPropName();
	void				SetParentArrayPropName(const char* pArrayPropName);

public:

	char* m_pVarName;
	SendPropType			m_RecvType;
	int						m_Flags;
	int						m_StringBufferSize;

private:

	bool					m_bInsideArray;

const void* m_pExtraData;

RecvProp* m_pArrayProp;
	ArrayLengthRecvProxyFn	m_ArrayLengthProxy;

	RecvVarProxyFn			m_ProxyFn;
	DataTableRecvVarProxyFn	m_DataTableProxyFn;

	RecvTable* m_pDataTable;
	int						m_Offset;

	int						m_ElementStride;
	int						m_nElements;

const char* m_pParentArrayPropName;
};

class RecvTable
{
public:

	typedef RecvProp	PropType;

	RecvTable();
	RecvTable(RecvProp* pProps, int nProps, char* pNetTableName);
	~RecvTable();

	void		Construct(RecvProp* pProps, int nProps, char* pNetTableName);

	int			GetNumProps();
	RecvProp* GetProp(int i);

	const char* GetName();

void		SetInitialized(bool bInitialized);
	bool		IsInitialized() const;

void		SetInMainList(bool bInList);
	bool		IsInMainList() const;

public:

RecvProp* m_pProps;
	int				m_nProps;

CRecvDecoder* m_pDecoder;

	char* m_pNetTableName;

private:

	bool			m_bInitialized;
	bool			m_bInMainList;
};

inline int RecvTable::GetNumProps()
{
	return m_nProps;
}

inline RecvProp* RecvTable::GetProp(int i)
{
	return &m_pProps[i];
}

inline const char* RecvTable::GetName()
{
	return m_pNetTableName;
}

inline void RecvTable::SetInitialized(bool bInitialized)
{
	m_bInitialized = bInitialized;
}

inline bool RecvTable::IsInitialized() const
{
	return m_bInitialized;
}

inline void RecvTable::SetInMainList(bool bInList)
{
	m_bInMainList = bInList;
}

inline bool RecvTable::IsInMainList() const
{
	return m_bInMainList;
}

inline void RecvProp::InitArray(int nElements, int elementStride)
{
	m_RecvType = DPT_Array;
	m_nElements = nElements;
	m_ElementStride = elementStride;
}

inline int RecvProp::GetNumElements() const
{
	return m_nElements;
}

inline void RecvProp::SetNumElements(int nElements)
{
	m_nElements = nElements;
}

inline int RecvProp::GetElementStride() const
{
	return m_ElementStride;
}

inline void RecvProp::SetElementStride(int stride)
{
	m_ElementStride = stride;
}

inline int RecvProp::GetFlags() const
{
	return m_Flags;
}

inline const char* RecvProp::GetName() const
{
	return m_pVarName;
}

inline SendPropType RecvProp::GetType() const
{
	return m_RecvType;
}

inline RecvTable* RecvProp::GetDataTable() const
{
	return m_pDataTable;
}

inline void RecvProp::SetDataTable(RecvTable* pTable)
{
	m_pDataTable = pTable;
}

inline RecvVarProxyFn RecvProp::GetProxyFn() const
{
	return m_ProxyFn;
}

inline void RecvProp::SetProxyFn(RecvVarProxyFn fn)
{
	m_ProxyFn = fn;
}

inline DataTableRecvVarProxyFn RecvProp::GetDataTableProxyFn() const
{
	return m_DataTableProxyFn;
}

inline void RecvProp::SetDataTableProxyFn(DataTableRecvVarProxyFn fn)
{
	m_DataTableProxyFn = fn;
}

inline int RecvProp::GetOffset() const
{
	return m_Offset;
}

inline void RecvProp::SetOffset(int o)
{
	m_Offset = o;
}

inline RecvProp* RecvProp::GetArrayProp() const
{
	return m_pArrayProp;
}

inline void RecvProp::SetArrayProp(RecvProp* pProp)
{
	m_pArrayProp = pProp;
}

inline void RecvProp::SetArrayLengthProxy(ArrayLengthRecvProxyFn proxy)
{
	m_ArrayLengthProxy = proxy;
}

inline ArrayLengthRecvProxyFn RecvProp::GetArrayLengthProxy() const
{
	return m_ArrayLengthProxy;
}

inline bool RecvProp::IsInsideArray() const
{
	return m_bInsideArray;
}

inline void RecvProp::SetInsideArray()
{
	m_bInsideArray = true;
}

inline const void* RecvProp::GetExtraData() const
{
	return m_pExtraData;
}

inline void RecvProp::SetExtraData(const void* pData)
{
	m_pExtraData = pData;
}

inline const char* RecvProp::GetParentArrayPropName()
{
	return m_pParentArrayPropName;
}

inline void	RecvProp::SetParentArrayPropName(const char* pArrayPropName)
{
	m_pParentArrayPropName = pArrayPropName;
}