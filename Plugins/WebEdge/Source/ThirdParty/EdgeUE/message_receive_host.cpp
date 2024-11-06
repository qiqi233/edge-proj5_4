#include "message_receive_host.h"

#include <sstream>

#include "edge_ue_mgr.h"
#include "webview_item_ui.h"
#include "ue_message_interface.h"

message_receive_host::message_receive_host(const wchar_t* path,const wchar_t* id)
    : RootPath(path)
    , m_uniqueID(id)
{
    
}

HRESULT message_receive_host::testfunc(float Val)
{
    std::wostringstream ss;
    ss << L"testfunc函数输出 : "<< Val << std::endl;
    std::wstring s = ss.str();
    OutputDebugString(s.c_str());
    return S_OK; 
}

HRESULT message_receive_host::TestFunStr(BSTR str)
{
    std::wstring wStr(str);
    std::wostringstream ss;
    ss << L"TestFunStr函数输出 : "<< wStr.c_str() << std::endl;
    std::wstring s = ss.str();
    OutputDebugString(s.c_str());
    SysFreeString(str);
    return S_OK; 
}

STDMETHODIMP_(HRESULT __stdcall) message_receive_host::QuitGame()
{
    edge_ue_mgr::instance()->get_message_interface()->QuitGame();
    return S_OK;
}


HRESULT message_receive_host::GetTypeInfoCount(UINT* pctinfo)
{
    *pctinfo = 1;
    return S_OK;
}

HRESULT message_receive_host::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
    if (0 != iTInfo)
    {
        return TYPE_E_ELEMENTNOTFOUND;
    }
    if (!m_typeLib)
    {
        //D:\\waibao\\EdgeProj5_1\\Binaries\\Win64\\/
        std::wstring filePath = RootPath + L"EdgeUE.tlb";
        RETURN_IF_FAILED(LoadTypeLib(filePath.c_str(), &m_typeLib));
    }
    return m_typeLib->GetTypeInfoOfGuid(__uuidof(Ihost_object_sample), ppTInfo);
}

HRESULT message_receive_host::GetIDsOfNames(const IID& riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid,
    DISPID* rgDispId)
{
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
    return typeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
}

HRESULT message_receive_host::Invoke(DISPID dispIdMember, const IID& riid, LCID lcid, WORD wFlags,
    DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
    return typeInfo->Invoke(
        this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}
