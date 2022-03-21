// DirectXStuff.hpp (Header Only)
// Feb 2020
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifdef _DEBUG
#define DEBUG_ENABLED true
#else
#define DEBUG_ENABLED false
#endif

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

namespace DirectXStuff {


	// General.

	void FailBail(const wchar_t* ErrorMessage, const wchar_t* ErrorTitle) {
		MessageBox(NULL, ErrorMessage, ErrorTitle, NULL);
		exit(-1);
	}
	
	void ResultCheck(HRESULT Result, const wchar_t* ErrorMessage, const wchar_t* ErrorTitle) {
		if (Result != S_OK) {
			MessageBox(NULL, ErrorMessage, ErrorTitle, NULL);
			exit(-1);
		}
	}

	class UnknownInterface {

	protected:

		IUnknown* pInterface;

		UnknownInterface() : pInterface{ nullptr } {

		}

		~UnknownInterface() {
			if (this->pInterface != nullptr) {
				this->pInterface->Release();
				this->pInterface = nullptr;
			}
		}

	public:

		virtual IUnknown* GetInterface() = 0;

	};


	// Direct3D.

	class Blob : UnknownInterface {

	public:

		ID3DBlob* GetInterface() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<ID3DBlob*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};


	// Direct3D 12.

	class D3D12DebugController : UnknownInterface {

	public:

		D3D12DebugController() {
			if (DEBUG_ENABLED == true) {
				HRESULT Result{ S_OK };

				Result = D3D12GetDebugInterface(__uuidof(ID3D12Debug3), reinterpret_cast<void**>(&this->pInterface));

				ResultCheck(Result, L"D3D12GetDebugInterface() failed.", L"DirectXStuff::D3D12DebugController Error");

				reinterpret_cast<ID3D12Debug3*>(this->pInterface)->EnableDebugLayer();

				reinterpret_cast<ID3D12Debug3*>(this->pInterface)->SetEnableGPUBasedValidation(true);
			}
		}

		ID3D12Debug3* GetInterface() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<ID3D12Debug3*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};

	class Device : UnknownInterface {

	public:

		Device(D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_12_1, const wchar_t* ObjectName = L"Device") {
			HRESULT Result{ S_OK };

			Result = D3D12CreateDevice(NULL, FeatureLevel, __uuidof(ID3D12Device6), reinterpret_cast<void**>(&this->pInterface));

			ResultCheck(Result, L"D3D12CreateDevice() failed.", L"DirectXStuff::Device Error");

			Result = reinterpret_cast<ID3D12Device6*>(this->pInterface)->SetName(ObjectName);

			ResultCheck(Result, L"SetName() failed.", L"DirectXStuff::Device Error");
		}

		ID3D12Device6* GetInterface() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<ID3D12Device6*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};

	class CommandQueue : UnknownInterface {

	public:

		CommandQueue(ID3D12Device6* pDevice, D3D12_COMMAND_QUEUE_PRIORITY Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			D3D12_COMMAND_LIST_TYPE QueueType = D3D12_COMMAND_LIST_TYPE_DIRECT, const wchar_t* ObjectName = L"CommandQueue",
			unsigned __int32 NodeMask = 0) {
			D3D12_COMMAND_QUEUE_DESC CommandQueueDescription{};
			CommandQueueDescription.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			CommandQueueDescription.NodeMask = NodeMask;
			CommandQueueDescription.Priority = Priority;
			CommandQueueDescription.Type = QueueType;

			HRESULT Result{ S_OK };

			Result = pDevice->CreateCommandQueue(&CommandQueueDescription, __uuidof(ID3D12CommandQueue),
				reinterpret_cast<void**>(&this->pInterface));

			ResultCheck(Result, L"CreateCommandQueue() failed.", L"DirectXStuff::CommandQueue Error");

			Result = reinterpret_cast<ID3D12CommandQueue*>(this->pInterface)->SetName(ObjectName);

			ResultCheck(Result, L"SetName() failed.", L"DirectXStuff::CommandQueue Error");
		}

		ID3D12CommandQueue* GetInterface() {
			if (this != nullptr) {
				return reinterpret_cast<ID3D12CommandQueue*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};
	
	class CommandAllocator : UnknownInterface {

	public:

		CommandAllocator(ID3D12Device6* pDevice, D3D12_COMMAND_LIST_TYPE CommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT,
			const wchar_t* ObjectName = L"CommandAllocator") {
			HRESULT Result{ S_OK };

			Result = pDevice->CreateCommandAllocator(CommandListType, __uuidof(ID3D12CommandAllocator),
				reinterpret_cast<void**>(&this->pInterface));

			ResultCheck(Result, L"CreatedCommandAllocator() failed.", L"DirectXStuff::CommandAllocator Error");

			Result = reinterpret_cast<ID3D12CommandAllocator*>(this->pInterface)->SetName(ObjectName);

			ResultCheck(Result, L"SetName() failed.", L"DirectXStuff::CommandAllocator Error");
		}

		ID3D12CommandAllocator* GetInterface() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<ID3D12CommandAllocator*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};

	class DescriptorHeap : UnknownInterface {

	public:

		DescriptorHeap(ID3D12Device6* pDevice, unsigned __int32 DescriptorCount = 128u,
			const wchar_t* ObjectName = L"DescriptorHeap", unsigned __int32 NodeMask = 0u) {
			D3D12_DESCRIPTOR_HEAP_DESC DecsriptorHeapDescription{};
			DecsriptorHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			DecsriptorHeapDescription.NodeMask = NodeMask;
			DecsriptorHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			DecsriptorHeapDescription.NumDescriptors = DescriptorCount;

			HRESULT Result{ S_OK };

			Result = pDevice->CreateDescriptorHeap(&DecsriptorHeapDescription, __uuidof(ID3D12DescriptorHeap),
				reinterpret_cast<void**>(&this->pInterface));

			ResultCheck(Result, L"CreateDescriptorHeap() failed.", L"DirectXStuff::DescriptorHeap Error");

			Result = reinterpret_cast<ID3D12DescriptorHeap*>(this->pInterface)->SetName(ObjectName);

			ResultCheck(Result, L"SetName() failed.", L"DirectXStuff::DescriptorHeap Error");
		}

		ID3D12DescriptorHeap* GetInterface() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<ID3D12DescriptorHeap*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};

	class Fence : UnknownInterface {

	public:

		Fence(ID3D12Device6* pDevice, const wchar_t* ObjectName = L"Fence") {
			HRESULT Result{ S_OK };

			Result = pDevice->CreateFence(0u, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence1),
				reinterpret_cast<void**>(&this->pInterface));

			ResultCheck(Result, L"CreateFence() failed.", L"DirectXStuff::Fence Error");

			Result = reinterpret_cast<ID3D12Fence1*>(this->pInterface)->SetName(ObjectName);

			ResultCheck(Result, L"SetName() failed.", L"DirectXStuff::Fence Error");
		}

		ID3D12Fence1* GetInterface() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<ID3D12Fence1*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};

	class RootSignature : UnknownInterface {
	
	public:

		RootSignature(ID3D12Device6* pDevice, ID3DBlob* pRootSignatureBlob,	ID3DBlob* pRootSignatureErrorBlob,
			unsigned __int32 PipelineResourceCount = 1u, const wchar_t* ObjectName = L"RootSignature",
			unsigned __int32 NodeMask = 0u) {
			D3D12_DESCRIPTOR_RANGE1 UAVDescriptorRange[1]{};
			UAVDescriptorRange[0].BaseShaderRegister = 0u;
			UAVDescriptorRange[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
			UAVDescriptorRange[0].NumDescriptors = PipelineResourceCount;
			UAVDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			UAVDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			UAVDescriptorRange[0].RegisterSpace = 0u;

			D3D12_ROOT_DESCRIPTOR_TABLE1 DescriptorTable{};
			DescriptorTable.NumDescriptorRanges = 1u;
			DescriptorTable.pDescriptorRanges = UAVDescriptorRange;

			D3D12_ROOT_CONSTANTS RootConstants{};
			RootConstants.Num32BitValues = 60u;
			RootConstants.RegisterSpace = 0u;
			RootConstants.ShaderRegister = 0u;

			D3D12_ROOT_PARAMETER1 RootParameters[2]{};
			RootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			RootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			RootParameters[0].DescriptorTable = DescriptorTable;
			RootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			RootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			RootParameters[1].Constants = RootConstants;

			D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootSignatureDescription{};
			RootSignatureDescription.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
			RootSignatureDescription.Desc_1_1.NumStaticSamplers = 0u;
			RootSignatureDescription.Desc_1_1.pStaticSamplers = nullptr;
			RootSignatureDescription.Desc_1_1.NumParameters = 2u;
			RootSignatureDescription.Desc_1_1.pParameters = RootParameters;
			RootSignatureDescription.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

			HRESULT Result{ S_OK };

			Result = D3D12SerializeVersionedRootSignature(&RootSignatureDescription, &pRootSignatureBlob,
				&pRootSignatureErrorBlob);

			ResultCheck(Result, L"D3D12SerializeVersionedRootSignature() failed.", L"DirectXStuff::RootSignature Error");

			Result = pDevice->CreateRootSignature(NodeMask, pRootSignatureBlob->GetBufferPointer(),
				pRootSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature),
				reinterpret_cast<void**>(&this->pInterface));

			ResultCheck(Result, L"CreateRootSignature() failed.", L"DirectXStuff::RootSignature Error");

			Result = reinterpret_cast<ID3D12RootSignature*>(this->pInterface)->SetName(ObjectName);

			ResultCheck(Result, L"SetName() failed.", L"DirectXStuff::RootSignature Error");
		}

		ID3D12RootSignature* GetInterface() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<ID3D12RootSignature*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};

	class PipelineState : UnknownInterface {

	public:

		PipelineState(ID3D12Device6* pDevice, ID3D12RootSignature* pRootSignature, unsigned __int32 SizeOfShaderBytecodeArray,
			void* ShaderBytecodeArray, const wchar_t* ObjectName = L"PipelineState", unsigned __int32 NodeMask = 0u) {
			D3D12_SHADER_BYTECODE ShaderByteCode{};
			ShaderByteCode.pShaderBytecode = ShaderBytecodeArray;
			ShaderByteCode.BytecodeLength = SizeOfShaderBytecodeArray;

			D3D12_COMPUTE_PIPELINE_STATE_DESC ComputePipelineStateDescription{};
			ComputePipelineStateDescription.CS = ShaderByteCode;
			ComputePipelineStateDescription.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
			ComputePipelineStateDescription.NodeMask = NodeMask;
			ComputePipelineStateDescription.pRootSignature = pRootSignature;

			HRESULT Result{ S_OK };

			Result = pDevice->CreateComputePipelineState(&ComputePipelineStateDescription, __uuidof(ID3D12PipelineState),
				reinterpret_cast<void**>(&this->pInterface));

			ResultCheck(Result, L"CreateComputePipelineState() failed.", L"DirectXStuff::PipelineState Error");

			Result = reinterpret_cast<ID3D12PipelineState*>(this->pInterface)->SetName(ObjectName);

			ResultCheck(Result, L"this->pPipelineStateInterface->SetName failed.", L"DirectXStuff::PipelineState Error");
		}

		ID3D12PipelineState* GetInterface() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<ID3D12PipelineState*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};

	class GraphicsCommandList : UnknownInterface {

	public:

		GraphicsCommandList(ID3D12Device6* pDevice, ID3D12CommandAllocator* pCommandAllocator,
			D3D12_COMMAND_LIST_TYPE CommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT,
			const wchar_t* ObjectName = L"GraphicsCommandList", unsigned __int32 NodeMask = 0u) {
			HRESULT Result{ S_OK };

			Result = pDevice->CreateCommandList(NodeMask, CommandListType, pCommandAllocator, nullptr,
				__uuidof(ID3D12GraphicsCommandList5), reinterpret_cast<void**>(&this->pInterface));

			ResultCheck(Result, L"CreateCommandList() failed.", L"DirectXStuff::GraphicsCommandList Error");

			Result = reinterpret_cast<ID3D12GraphicsCommandList5*>(this->pInterface)->SetName(ObjectName);

			ResultCheck(Result, L"SetName() failed.", L"DirectXStuff::GraphicsCommandList Error");
		}

		ID3D12GraphicsCommandList5* GetInterface() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<ID3D12GraphicsCommandList5*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

		ID3D12CommandList* GetListForSubmission() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<ID3D12CommandList*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};

	struct UATexture1DConfig {
		D3D12_CPU_PAGE_PROPERTY CPUPageProperty{ D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE };
		unsigned __int32 NodeMask{ 0u };
		D3D12_MEMORY_POOL MemoryPool{ D3D12_MEMORY_POOL_L1 };
		DXGI_FORMAT TextureFormat{ DXGI_FORMAT_R32_TYPELESS };
		unsigned __int32 TextureWidth{ 1u };
		D3D12_RESOURCE_STATES InitialResourceState{ D3D12_RESOURCE_STATE_COMMON };
	};

	class UATexture1D : UnknownInterface {

	public:

		UATexture1D(ID3D12Device6* pDevice, UATexture1DConfig Config,
			const wchar_t* ObjectName = L"UATexture1D") {
			HRESULT Result{ S_OK };

			D3D12_HEAP_PROPERTIES HeapProperties{};
			HeapProperties.CPUPageProperty = Config.CPUPageProperty;
			HeapProperties.CreationNodeMask = Config.NodeMask;
			HeapProperties.MemoryPoolPreference = Config.MemoryPool;
			HeapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
			HeapProperties.VisibleNodeMask = Config.NodeMask;

			D3D12_RESOURCE_DESC ResourceDescription{};
			ResourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
			ResourceDescription.DepthOrArraySize = 1u;
			ResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
			ResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;
			ResourceDescription.Format = Config.TextureFormat;
			ResourceDescription.Height = 1u;
			ResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			ResourceDescription.MipLevels = 1u;
			ResourceDescription.SampleDesc.Count = 1u;
			ResourceDescription.SampleDesc.Quality = 0u;
			ResourceDescription.Width = Config.TextureWidth;

			Result = pDevice->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE,
				&ResourceDescription, Config.InitialResourceState, nullptr, __uuidof(ID3D12Resource),
				reinterpret_cast<void**>(&this->pInterface));

			ResultCheck(Result, L"CreateCommittedResource() failed.", L"DirectXStuff::UATexture1D Error");

			Result = reinterpret_cast<ID3D12Resource*>(this->pInterface)->SetName(ObjectName);

			ResultCheck(Result, L"SetName() failed.", L"DirectXStuff::UATexture1D Error");
		}

		ID3D12Resource* GetInterface() {
			if (this->pInterface != nullptr)
			{
				return reinterpret_cast<ID3D12Resource*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};

	struct UATexture2DConfig {
		D3D12_CPU_PAGE_PROPERTY CPUPageProperty{ D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE };
		unsigned __int32 NodeMask{ 0u };
		D3D12_MEMORY_POOL MemoryPool{ D3D12_MEMORY_POOL_L1 };
		DXGI_FORMAT TextureFormat{ DXGI_FORMAT_R32_TYPELESS };
		unsigned __int32 TextureWidth{ 1u };
		unsigned __int32 TextureHeight{ 1u };
		D3D12_RESOURCE_STATES InitialResourceState{ D3D12_RESOURCE_STATE_COMMON };
	};

	class UATexture2D : UnknownInterface {

	public:
		
		UATexture2D(ID3D12Device6* pDevice, UATexture2DConfig Config,
			const wchar_t* ObjectName = L"UATexture2D") {
			HRESULT Result{ S_OK };

			D3D12_HEAP_PROPERTIES HeapProperties{};
			HeapProperties.CPUPageProperty = Config.CPUPageProperty;
			HeapProperties.CreationNodeMask = Config.NodeMask;
			HeapProperties.MemoryPoolPreference = Config.MemoryPool;
			HeapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
			HeapProperties.VisibleNodeMask = Config.NodeMask;

			D3D12_RESOURCE_DESC ResourceDescription{};
			ResourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
			ResourceDescription.DepthOrArraySize = 1u;
			ResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			ResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;
			ResourceDescription.Format = Config.TextureFormat;
			ResourceDescription.Height = Config.TextureHeight;
			ResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			ResourceDescription.MipLevels = 1u;
			ResourceDescription.SampleDesc.Count = 1u;
			ResourceDescription.SampleDesc.Quality = 0u;
			ResourceDescription.Width = Config.TextureWidth;

			Result = pDevice->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE,
				&ResourceDescription, Config.InitialResourceState, nullptr, __uuidof(ID3D12Resource),
				reinterpret_cast<void**>(&this->pInterface));

			ResultCheck(Result, L"CreateCommittedResource() failed.", L"DirectXStuff::UATexture2D Error");

			Result = reinterpret_cast<ID3D12Resource*>(this->pInterface)->SetName(ObjectName);

			ResultCheck(Result, L"SetName() failed.", L"DirectXStuff::UATexture2D Error");
		}

		ID3D12Resource* GetInterface() {
			if (this->pInterface != nullptr)
			{
				return reinterpret_cast<ID3D12Resource*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};

	struct UATexture3DConfig {
		D3D12_CPU_PAGE_PROPERTY CPUPageProperty{ D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE };
		unsigned __int32 NodeMask{ 0u };
		D3D12_MEMORY_POOL MemoryPool{ D3D12_MEMORY_POOL_L1 };
		DXGI_FORMAT TextureFormat{ DXGI_FORMAT_R32_TYPELESS };
		unsigned __int32 TextureWidth{ 1u };
		unsigned __int32 TextureHeight{ 1u };
		unsigned __int32 TextureDepth{ 1u };
		D3D12_RESOURCE_STATES InitialResourceState{ D3D12_RESOURCE_STATE_COMMON };
	};

	class UATexture3D : UnknownInterface {

	public:

		UATexture3D(ID3D12Device6* pDevice, UATexture3DConfig Config,
			const wchar_t* ObjectName = L"UATexture3D") {
			HRESULT Result{ S_OK };

			D3D12_HEAP_PROPERTIES HeapProperties{};
			HeapProperties.CPUPageProperty = Config.CPUPageProperty;
			HeapProperties.CreationNodeMask = Config.NodeMask;
			HeapProperties.MemoryPoolPreference = Config.MemoryPool;
			HeapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
			HeapProperties.VisibleNodeMask = Config.NodeMask;

			D3D12_RESOURCE_DESC ResourceDescription{};
			ResourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
			ResourceDescription.DepthOrArraySize = Config.TextureDepth;
			ResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			ResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;
			ResourceDescription.Format = Config.TextureFormat;
			ResourceDescription.Height = Config.TextureHeight;
			ResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			ResourceDescription.MipLevels = 1u;
			ResourceDescription.SampleDesc.Count = 1u;
			ResourceDescription.SampleDesc.Quality = 0u;
			ResourceDescription.Width = Config.TextureWidth;

			Result = pDevice->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE,
				&ResourceDescription, Config.InitialResourceState, nullptr, __uuidof(ID3D12Resource),
				reinterpret_cast<void**>(&this->pInterface));

			ResultCheck(Result, L"CreateCommittedResource() failed.", L"DirectXStuff::UATexture3D Error");

			Result = reinterpret_cast<ID3D12Resource*>(this->pInterface)->SetName(ObjectName);

			ResultCheck(Result, L"SetName() failed.", L"DirectXStuff::UATexture3D Error");
		}

		ID3D12Resource* GetInterface() {
			if (this->pInterface != nullptr)
			{
				return reinterpret_cast<ID3D12Resource*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};


	// DirectX Graphics Infrastructure.

	class DXGIDebugController : UnknownInterface {

	public:

		DXGIDebugController() {
			if (DEBUG_ENABLED == true) {
				HRESULT Result{ S_OK };

				Result = DXGIGetDebugInterface1(NULL, __uuidof(IDXGIDebug1), reinterpret_cast<void**>(&this->pInterface));

				ResultCheck(Result, L"DXGIGetDebugInterface() failed.", L"DirectXStuff::DXGIDebugController Error");
			}
		}

		IDXGIDebug1* GetInterface() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<IDXGIDebug1*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

		void ReportDebugInfo() {
			if ((this->pInterface != nullptr) && (DEBUG_ENABLED == true)) {
					HRESULT Result{ S_OK };

					Result = reinterpret_cast<IDXGIDebug1*>(this->pInterface)->ReportLiveObjects(DXGI_DEBUG_ALL,
						DXGI_DEBUG_RLO_ALL);

					ResultCheck(Result, L"ReportLiveObjects() failed.", L"DirectXStuff::DebugController Error");
			}
		}
		
	};

	class Factory : UnknownInterface {

	public:

		Factory() {
			HRESULT Result{ S_OK };

			Result = CreateDXGIFactory(__uuidof(IDXGIFactory7), reinterpret_cast<void**>(&this->pInterface));

			ResultCheck(Result, L"CreateDXGIFactory() failed.", L"DXGIStuff::Factory Error");
		}

		IDXGIFactory7* GetInterface() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<IDXGIFactory7*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};
	
	class SwapChain : UnknownInterface {
	
	public:
		SwapChain(IDXGIFactory7* pFactory, ID3D12CommandQueue* pCommandQueue, HWND WindowHandle, unsigned __int32 PixelWidth,
			unsigned __int32 PixelHeight, unsigned __int32 BufferCount = 2u, DXGI_FORMAT BufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM) {
			DXGI_SWAP_CHAIN_DESC1 SwapChainDescription{};
			SwapChainDescription.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
			SwapChainDescription.BufferCount = BufferCount;
			SwapChainDescription.BufferUsage = DXGI_USAGE_BACK_BUFFER;
			SwapChainDescription.Flags = NULL;
			SwapChainDescription.Format = BufferFormat;
			SwapChainDescription.Height = PixelHeight;
			SwapChainDescription.SampleDesc.Count = 1u;
			SwapChainDescription.SampleDesc.Quality = 0u;
			SwapChainDescription.Scaling = DXGI_SCALING_NONE;
			SwapChainDescription.Stereo = FALSE;
			SwapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			SwapChainDescription.Width = PixelWidth;

			HRESULT Result{ S_OK };

			Result = pFactory->CreateSwapChainForHwnd(pCommandQueue, WindowHandle, &SwapChainDescription, nullptr, nullptr,
				reinterpret_cast<IDXGISwapChain1**>(&this->pInterface));

			ResultCheck(Result, L"CreateSwapChainForHwnd() failed.", L"DXGIStuff::SwapChain Error");
		}

		IDXGISwapChain4* GetInterface() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<IDXGISwapChain4*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};


	// DirectX Shader Compiler.

	class DXCLibrary : UnknownInterface {

	public:

		DXCLibrary() {
			HRESULT Result{ S_OK };
			
			Result = DxcCreateInstance(CLSID_DxcLibrary, __uuidof(IDxcLibrary),
				reinterpret_cast<void**>(&this->pInterface));

			ResultCheck(Result, L"DxcCreateInstance() failed.", L"DirectXStuff::DXCLibrary Error");
		}

		IDxcLibrary* GetInterface() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<IDxcLibrary*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};

	class DXCCompiler : UnknownInterface {

	public:

		DXCCompiler() {
			HRESULT Result{ S_OK };

			Result = DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler),
				reinterpret_cast<void**>(&this->pInterface));

			ResultCheck(Result, L"DxcCreateInstance() failed.", L"DirectXStuff::DXCCompiler Error");
		}

		IDxcCompiler2* GetInterface() {
			if (this->pInterface != nullptr) {
				return reinterpret_cast<IDxcCompiler2*>(this->pInterface);
			}
			else {
				return nullptr;
			}
		}

	};

	struct ShaderConfig {
		const wchar_t* ShaderFileName{ L"Shader.hlsl" };
		unsigned __int32 ShaderFileEncoding{ CP_UTF8 };
		const wchar_t* ShaderEntryPoint{ L"Main" };
		const wchar_t* TargetProfile{ L"cs_6_3" };
	};

	class Shader {
	
	protected:

		IDxcBlobEncoding* pSourceBlob;
		IDxcOperationResult* pOperationResult;
		IDxcBlob* pCompiledShaderBlob;
		IDxcIncludeHandler* pIncludeHandler;

	public:

		Shader(IDxcLibrary* pDXCLibrary, IDxcCompiler* pDXCCompiler, ShaderConfig Config) : 
			pSourceBlob{ nullptr }, pOperationResult{ nullptr }, pCompiledShaderBlob{ nullptr },
			pIncludeHandler{ nullptr } {
			HRESULT Result{ S_OK };

			Result = pDXCLibrary->CreateBlobFromFile(Config.ShaderFileName, &Config.ShaderFileEncoding,
				&pSourceBlob);

			ResultCheck(Result, L"CreateBlobFromFile() failed.", L"DirectXStuff::Shader Error");

			Result = pDXCCompiler->Compile(pSourceBlob, Config.ShaderFileName, Config.ShaderEntryPoint,
				Config.TargetProfile, NULL, 0u, NULL, 0u, pIncludeHandler, &pOperationResult);

			ResultCheck(Result, L"Compile() failed.", L"DirectXStuff::Shader Error");

			pOperationResult->GetStatus(&Result);

			ResultCheck(Result, L"Compile() failed.", L"DirectXStuff::Shader Error");

			Result = pOperationResult->GetResult(&pCompiledShaderBlob);

			ResultCheck(Result, L"Compile() failed.", L"DirectXStuff::Shader Error");
		}
		
		unsigned __int32 GetShaderByteCodeSize() {
			if (pCompiledShaderBlob != nullptr) {
				return static_cast<unsigned __int32>(pCompiledShaderBlob->GetBufferSize());
			}
			else {
				return 0u;
			}
		}

		void* GetShaderByteCode() {
			if (pCompiledShaderBlob != nullptr) {
				return pCompiledShaderBlob->GetBufferPointer();
			}
			else {
				return nullptr;
			}
		}

		~Shader() {
			if (this->pIncludeHandler != nullptr) {
				this->pIncludeHandler->Release();
				this->pIncludeHandler = nullptr;
			}

			if (this->pCompiledShaderBlob != nullptr) {
				this->pCompiledShaderBlob->Release();
				this->pCompiledShaderBlob = nullptr;
			}

			if (this->pOperationResult != nullptr) {
				this->pOperationResult->Release();
				this->pOperationResult = nullptr;
			}

			if (this->pSourceBlob != nullptr) {
				this->pSourceBlob->Release();
				this->pSourceBlob = nullptr;
			}
		}

	};

}
