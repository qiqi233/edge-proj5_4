#pragma once
#include <windows.h>
#include <wil/com.h>
#include <winrt/base.h>
#include <wrl.h>
#include <functional>
#include "message_host_impl_h.h"

class webview_item_ui;
class message_receive_host:public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, Ihost_object_sample, IDispatch>
{
public:
    message_receive_host(const wchar_t* path,const wchar_t* id);
    STDMETHODIMP testfunc(float Val) override;
    STDMETHODIMP TestFunStr(BSTR str) override;
    STDMETHODIMP QuitGame() override;
    //系统必须重写的接口
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override;
    STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;
    STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;
    STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;

private:
    std::wstring RootPath;
    wil::com_ptr<ITypeLib> m_typeLib;
    std::wstring m_uniqueID;
};
