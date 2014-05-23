#pragma once

#include <Windows.h>

#include <d3d9.h>
#include <evr.h>

#include <cstdio>

static const GUID CLSID_CustomEVRPresenter =
{ 0xb21f3368, 0x7260, 0x4b45, { 0x91, 0x79, 0xba, 0x51, 0x59, 0xe, 0x3b, 0x9e } };

MIDL_INTERFACE("576DCFD9-3C6B-4317-A635-2A1494E08B75")
IEVRPresenterCallback : public IUnknown
{
public:
    STDMETHOD(PresentSurfaceCB)(IDirect3DSurface9 *pSurface) = 0;
};

MIDL_INTERFACE("F3AB6A07-5A21-4034-908A-A9F48FAC2F63")
IEVRPresenterRegisterCallback : public IUnknown
{
public:
    STDMETHOD(RegisterCallback)(IEVRPresenterCallback *pCallback) = 0;
};

MIDL_INTERFACE("9B594575-859E-4848-812B-441406F94E5F")
IEVRPresenterSettings : public IUnknown
{
public:
    STDMETHOD(SetBufferCount)(int bufferCount) = 0;
};

class EVRPresenter : public IEVRPresenterCallback
{
public:
    EVRPresenter() : _surface(nullptr), _refCount(0) {}

    virtual HRESULT STDMETHODCALLTYPE PresentSurfaceCB(IDirect3DSurface9* pSurface)
    {
        if (_surface != pSurface)
        {
            D3DSURFACE_DESC desc;
            pSurface->GetDesc(&desc);
            fprintf(stderr, "New surface (%dx%d)!\n", desc.Width, desc.Height);
        }

        _surface = pSurface;
        fprintf(stderr, "New frame\n");
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** pvObject)
    {
        if (pvObject == NULL) return E_POINTER;

        if (riid == __uuidof(IEVRPresenterCallback))
        {
            *pvObject = static_cast<IEVRPresenterCallback*>(this);
            AddRef();
            return S_OK;
        }
        else if (riid == IID_IUnknown)
        {
            *pvObject = static_cast<IUnknown*>(this);
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return _InterlockedIncrement(&_refCount);
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        long ref = _InterlockedDecrement(&_refCount);
        if (ref == 0) delete this;
        return ref;
    }

private:
    IDirect3DSurface9* _surface;
    long _refCount;
};