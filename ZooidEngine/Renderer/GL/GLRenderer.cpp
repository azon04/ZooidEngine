#include "GLRenderer.h"

#include "ZEngine.h"
#include "Enums.h"
#include "Platform/Platform.h"

#if defined(_WIN32) || defined(_WIN64)
	#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include "GLFW/glfw3native.h"
#include <cstring>

namespace ZE 
{

	GLRenderer::GLRenderer()
	{
		HashFeatureToRealGLVar.put(RendererFeature::DEPTH_TEST, GL_DEPTH_TEST);
		HashFeatureToRealGLVar.put(RendererFeature::STENCIL_TEST, GL_STENCIL_TEST);
		HashFeatureToRealGLVar.put(RendererFeature::BLEND, GL_BLEND);
		HashFeatureToRealGLVar.put(RendererFeature::FACE_CULING, GL_CULL_FACE);

		HashCompareFuncToRealGLVar.put(ERendererCompareFunc::ALWAYS, GL_ALWAYS);
		HashCompareFuncToRealGLVar.put(ERendererCompareFunc::NEVER, GL_NEVER);
		HashCompareFuncToRealGLVar.put(ERendererCompareFunc::LESS, GL_LESS);
		HashCompareFuncToRealGLVar.put(ERendererCompareFunc::EQUAL, GL_EQUAL);
		HashCompareFuncToRealGLVar.put(ERendererCompareFunc::LEQUAL, GL_LEQUAL);
		HashCompareFuncToRealGLVar.put(ERendererCompareFunc::GREATER, GL_GREATER);
		HashCompareFuncToRealGLVar.put(ERendererCompareFunc::NOTEQUAL, GL_NOTEQUAL);
		HashCompareFuncToRealGLVar.put(ERendererCompareFunc::GEQUAL, GL_GEQUAL);

		HashBlendFactorToRealGLVar.put(ERendererBlendFactor::ZERO, GL_ZERO);
		HashBlendFactorToRealGLVar.put(ERendererBlendFactor::ONE, GL_ONE);
		HashBlendFactorToRealGLVar.put(ERendererBlendFactor::SRC_COLOR, GL_SRC_COLOR);
		HashBlendFactorToRealGLVar.put(ERendererBlendFactor::ONE_MINUS_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
		HashBlendFactorToRealGLVar.put(ERendererBlendFactor::DST_COLOR, GL_DST_COLOR);
		HashBlendFactorToRealGLVar.put(ERendererBlendFactor::ONE_MINUS_DST_COLOR, GL_ONE_MINUS_DST_COLOR);
		HashBlendFactorToRealGLVar.put(ERendererBlendFactor::SRC_ALPHA, GL_SRC_ALPHA);
		HashBlendFactorToRealGLVar.put(ERendererBlendFactor::ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		HashBlendFactorToRealGLVar.put(ERendererBlendFactor::DST_ALPHA, GL_DST_ALPHA);
		HashBlendFactorToRealGLVar.put(ERendererBlendFactor::ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
		HashBlendFactorToRealGLVar.put(ERendererBlendFactor::CONSTANT_COLOR, GL_CONSTANT_COLOR);
		HashBlendFactorToRealGLVar.put(ERendererBlendFactor::ONE_MINUS_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
		HashBlendFactorToRealGLVar.put(ERendererBlendFactor::CONSTANT_ALPHA, GL_CONSTANT_ALPHA);
		HashBlendFactorToRealGLVar.put(ERendererBlendFactor::ONE_MINUS_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
		
		HashCullFace[ECullFace::FRONT] = GL_FRONT;
		HashCullFace[ECullFace::BACK] = GL_BACK;
		HashCullFace[ECullFace::FRONT_AND_BACK] = GL_FRONT_AND_BACK;

		HashFaceFrontOrder[EFaceFrontOrder::CCW] = GL_CCW;
		HashFaceFrontOrder[EFaceFrontOrder::CW] = GL_CW;
	}

	void GLRenderer::Setup()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		
		m_window = glfwCreateWindow(WIDTH, HEIGHT, "ZooidEngine - Windows", nullptr, nullptr);
		if (!m_window) 
		{
			glfwTerminate();
			ZASSERT(true, "Failed to create GLFW window");
			return;
		}
		glfwMakeContextCurrent(m_window);

#if defined(_WIN32) || defined(_WIN64)
		// Setup WinWindow
		m_winWindow = glfwGetWin32Window(m_window);
#endif

		// Init glfw
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK) 
		{
			glfwTerminate();
			ZASSERT(false, "GL Init Error: %s", glewGetErrorString(err));
			return;
		}

		// Viewport setup
		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);

		// Enable Depth test
		EnableFeature(RendererFeature::DEPTH_TEST);

		// Enable Stencil Buffer
		EnableFeature(RendererFeature::STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		// Enable Face Culling
		EnableFeature(FACE_CULING);
		glFrontFace(GL_CCW);
	}

	void GLRenderer::BeginRender()
	{
		glViewport(0, 0, WIDTH, HEIGHT);
	}

	void GLRenderer::EndRender()
	{
		// Swap buffer
		glfwSwapBuffers(m_window);
	}

	void GLRenderer::Clean()
	{
		glfwTerminate();
	}

	void GLRenderer::ClearScreen()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glStencilMask(0xFF); // Before clearning Stencil buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00); // Disable writing to stencil buffer
	}

	void GLRenderer::Clear(UInt32 clearBits)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(getClearBitMask(clearBits));
	}

	void GLRenderer::ProcessShadowMapList(DrawList* drawList, bool bWithStatic)
	{
		// Current peter panning solution: Render with front face culled.
		glCullFace(GL_FRONT);

		glClear(GL_DEPTH_BUFFER_BIT);

		for (UInt32 i = 0; i < drawList->m_dynamicShadowObjSize; i++)
		{
			DrawEx(drawList, &drawList->m_dynamicShadowObjList[i], false);
		}

		if (bWithStatic)
		{
			for (UInt32 i = 0; i < drawList->m_staticShadowObjSize; i++)
			{
				DrawEx(drawList, &drawList->m_staticShadowObjList[i], false);
			}
		}

		glCullFace(GL_BACK);
	}

	void GLRenderer::ProcessDrawList(DrawList* drawList)
	{
		if (drawList->m_mainConstantBuffer)
		{
			drawList->m_mainConstantBuffer->bindAndRefresh();
		}

		if (drawList->m_lightConstantBuffer)
		{
			drawList->m_lightConstantBuffer->bindAndRefresh();
		}

		for (int i = 0; i < drawList->m_size; ++i)
		{
			ProcessShaderAction(drawList, &drawList->m_drawList[i]);
		}

		UInt32 blendSortIndices[MAX_SECONDPASS_DRAW_LIST];
		SortBlendShaderActions(drawList->m_cameraPosition, drawList->m_cameraDirection, drawList->m_secondPassDrawList, blendSortIndices, drawList->m_secondPassSize);
		for (int i = 0; i < drawList->m_secondPassSize; ++i)
		{
			ProcessShaderAction(drawList, &drawList->m_secondPassDrawList[blendSortIndices[i]]);
		}

		if (drawList->m_mainConstantBuffer)
		{
			drawList->m_mainConstantBuffer->unbind();
		}

		if (drawList->m_lightConstantBuffer)
		{
			drawList->m_lightConstantBuffer->unbind();
		}
	}

	void GLRenderer::ProcessShaderAction(DrawList* drawList, ShaderAction* shaderAction)
	{
		switch (shaderAction->getShaderActionType())
		{
		case SHADER_ACTION_DRAW:
			Draw(drawList, shaderAction);
			break;
		}
	}

	void GLRenderer::Draw(DrawList* drawList, ShaderAction* shaderAction)
	{
		DrawEx(drawList, shaderAction, true);
	}

	bool GLRenderer::IsClose()
	{
		return glfwWindowShouldClose(m_window) == 1;
	}

	void GLRenderer::PollEvent()
	{
		glfwPollEvents();
		if ( glfwWindowShouldClose(m_window) == 1 )
		{
			Platform::RequestExit(0);
		}
	}

	void GLRenderer::AcquireRenderThreadOwnership()
	{
		m_renderLock.lock();
		glfwMakeContextCurrent(m_window);
		m_currentThreadHasLock = ZE::getThreadId();
	}

	void GLRenderer::ReleaseRenderThreadOwnership()
	{	
		glfwMakeContextCurrent(nullptr);
		m_renderLock.unlock();
		m_currentThreadHasLock = ThreadId();
	}

	bool GLRenderer::HasRenderThreadOwnership()
	{
		return m_currentThreadHasLock == ZE::getThreadId();
	}

	void GLRenderer::EnableFeature(UInt32 feature)
	{
		glEnable(HashFeatureToRealGLVar[feature]);
	}

	void GLRenderer::DisableFeature(UInt32 feature)
	{
		glDisable(HashFeatureToRealGLVar[feature]);
	}

	bool GLRenderer::IsFeatureEnabled(UInt32 feature)
	{
		return glIsEnabled(HashFeatureToRealGLVar[feature]) == GL_TRUE;
	}

	void GLRenderer::ResetFeature(UInt32 feature)
	{
		if (feature == RendererFeature::DEPTH_TEST)
		{
			glDepthFunc(GL_LESS);
		}
		else if (feature == RendererFeature::STENCIL_TEST)
		{
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0x00);
		}
		else if (feature == RendererFeature::FACE_CULING)
		{
			glFrontFace(GL_CCW);
			glCullFace(GL_BACK);
		}
	}

	void GLRenderer::DrawEx(DrawList* drawList, ShaderAction* shaderAction, bool bWithShadow)
	{
		shaderAction->getShaderChain()->bind();

		for (int i = 0; i < shaderAction->m_shaderFeatures.length(); i++)
		{
			ShaderFeature& shaderFeature = shaderAction->m_shaderFeatures[i];

			bool bDefaultEnable = IsFeatureEnabled(shaderFeature.m_rendererFeature);

			ProcessShaderFeature(shaderFeature);

			// Save the default value for reverting the renderer feature
			shaderFeature.m_bFeatureEnabled = bDefaultEnable;
		}

		Int32 variableCount = shaderAction->m_shaderVariables.length();
		Int32 shadowMapOffset = 0;
		for (int i = 0; i < variableCount; i++)
		{
			ShaderVariable& shaderVariable = shaderAction->m_shaderVariables[i];
			switch (shaderVariable.VarType)
			{
			case SHADER_VAR_TYPE_FLOAT:
				shaderAction->getShaderChain()->setFloat(shaderVariable.VarName, shaderVariable.Float_value);
				break;
			case SHADER_VAR_TYPE_INT:
				shaderAction->getShaderChain()->setInt(shaderVariable.VarName, shaderVariable.Int_value);
				break;
			case SHADER_VAR_TYPE_VECTOR3:
				shaderAction->getShaderChain()->setVec3(shaderVariable.VarName, shaderVariable.Vec3_value);
				break;
			case SHADER_VAR_TYPE_MATRIX:
				shaderAction->getShaderChain()->setMat(shaderVariable.VarName, shaderVariable.Mat_value);
				break;
			case SHADER_VAR_TYPE_TEXTURE:
				shaderAction->getShaderChain()->setTexture(shaderVariable.VarName, shaderVariable.texture_value.Texture_data, shaderVariable.texture_value.Texture_index);
				glActiveTexture(GL_TEXTURE0 + shaderVariable.texture_value.Texture_index);
				shaderVariable.texture_value.Texture_data->bind();
				if (bWithShadow && shadowMapOffset <= shaderVariable.texture_value.Texture_index)
				{
					shadowMapOffset = shaderVariable.texture_value.Texture_index + 1;
				}
				break;
			case SHADER_VAR_TYPE_BLOCK_BUFFER:
				shaderAction->getShaderChain()->bindConstantBuffer(shaderVariable.VarName, shaderVariable.Constant_buffer);
			}
		}

		if (bWithShadow)
		{
			BindShadowMaps(drawList, shaderAction->getShaderChain(), shadowMapOffset);
		}

		GLenum drawTopology = GL_TRIANGLES;

		switch (shaderAction->getShaderChain()->getRenderTopology())
		{
		case TOPOLOGY_LINE:
			drawTopology = GL_LINES;
			break;
		case TOPOLOGY_TRIANGLE:
			drawTopology = GL_TRIANGLES;
			break;
		case TOPOLOGY_POINT:
			drawTopology = GL_POINTS;
			break;
		}

		shaderAction->getBufferArray()->bind();
		if (shaderAction->getBufferArray()->isUsingIndexBuffer())
		{
			glDrawElements(drawTopology, shaderAction->getVertexSize(), GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawArrays(drawTopology, 0, shaderAction->getVertexSize());
		}

		// Unbind textures, buffer array and shader
		for (int i = 0; i < shaderAction->m_shaderVariables.length(); i++)
		{
			ShaderVariable& shaderVariable = shaderAction->m_shaderVariables[i];
			switch (shaderVariable.VarType)
			{
			case SHADER_VAR_TYPE_TEXTURE:
				glActiveTexture(GL_TEXTURE0 + shaderVariable.texture_value.Texture_index);
				shaderVariable.texture_value.Texture_data->unbind();
				break;
			}
		}

		if (bWithShadow)
		{
			UnbindShadowMaps(drawList, shaderAction->getShaderChain(), shadowMapOffset);
		}

		// Revert Render Feature
		for (int i = 0; i < shaderAction->m_shaderFeatures.length(); i++)
		{
			ShaderFeature& shaderFeature = shaderAction->m_shaderFeatures[i];
			if (shaderFeature.m_bFeatureEnabled)
			{
				EnableFeature(shaderFeature.m_rendererFeature);
				ResetFeature(shaderFeature.m_rendererFeature);
			}
			else
			{
				DisableFeature(shaderFeature.m_rendererFeature);
			}
		}

		shaderAction->getBufferArray()->unbind();
		shaderAction->getShaderChain()->unbind();
	}

	void GLRenderer::BindShadowMaps(DrawList* _drawList, IShaderChain* shaderChain, int offset)
	{
		char buffer[25];
		for (UInt32 i = 0; i < _drawList->m_shadowMapSize; i++)
		{
			StringFunc::PrintToString(buffer, 25, "shadowMaps[%d]", i);
			shaderChain->setTexture(buffer, _drawList->m_shadowMap[i], offset + i);
			glActiveTexture(GL_TEXTURE0 + offset + i);
			_drawList->m_shadowMap[i]->bind();
		}
	}

	void GLRenderer::UnbindShadowMaps(DrawList* _drawList, IShaderChain* shaderChain, int offset)
	{
		for (UInt32 i = 0; i < _drawList->m_shadowMapSize; i++)
		{
			glActiveTexture(GL_TEXTURE0 + offset + i);
			_drawList->m_shadowMap[i]->unbind();
		}
	}

	void GLRenderer::ProcessShaderFeature(ShaderFeature& shaderFeature)
	{
		if (shaderFeature.m_bFeatureEnabled)
		{
			EnableFeature(shaderFeature.m_rendererFeature);

			if (shaderFeature.m_rendererFeature == RendererFeature::DEPTH_TEST)
			{

				if (shaderFeature.m_shaderFeatureVar.length() > 0)
				{
					glDepthFunc(HashCompareFuncToRealGLVar[shaderFeature.m_shaderFeatureVar[0].Uint_value]);
				}
			}
			else if (shaderFeature.m_rendererFeature == RendererFeature::STENCIL_TEST)
			{
				if (shaderFeature.m_shaderFeatureVar.length() > 0)
				{
					glStencilFunc(HashCompareFuncToRealGLVar[shaderFeature.m_shaderFeatureVar[0].Uint_value], 
						shaderFeature.m_shaderFeatureVar[1].Int_value, 
						shaderFeature.m_shaderFeatureVar[2].Uint_value);
					glStencilMask(shaderFeature.m_shaderFeatureVar[3].Uint_value);
				}
			}
			else if(shaderFeature.m_rendererFeature == RendererFeature::BLEND)
			{
				GLenum sourceFactor = HashBlendFactorToRealGLVar[shaderFeature.m_shaderFeatureVar[0].Uint_value];
				GLenum destFactor = HashBlendFactorToRealGLVar[shaderFeature.m_shaderFeatureVar[1].Uint_value];
				glBlendFunc(sourceFactor, destFactor);
			}
			else if (shaderFeature.m_rendererFeature == RendererFeature::FACE_CULING)
			{
				glFrontFace(HashFaceFrontOrder[shaderFeature.m_shaderFeatureVar[0].Uint_value]);
				glCullFace(HashCullFace[shaderFeature.m_shaderFeatureVar[1].Uint_value]);
			}
		}
		else
		{
			DisableFeature(shaderFeature.m_rendererFeature);
		}
	}

	UInt32 quickSortBlendPartition(Float32* squareDist, UInt32* arr, Int32 lo, Int32 hi)
	{
		Float32 pivot = squareDist[arr[hi]];
		Int32 i = lo - 1;
		for (Int32 j = lo; j < hi; j++)
		{
			if (squareDist[arr[j]] > pivot)
			{
				i++;
				UInt32 temp = arr[j];
				arr[j] = arr[i];
				arr[i] = temp;
			}
		}

		i++;
		UInt32 temp = arr[hi];
		arr[hi] = arr[i];
		arr[i] = temp;

		return i;
	}

	void quickSortBlend(Float32* squareDist, UInt32* arr, Int32 lo, Int32 hi)
	{
		if (lo < hi)
		{
			UInt32 p = quickSortBlendPartition(squareDist, arr, lo, hi);
			quickSortBlend(squareDist, arr, lo, p - 1);
			quickSortBlend(squareDist, arr, p + 1, hi);
		}
	}

	void GLRenderer::SortBlendShaderActions(const Vector3& cameraPosition, const Vector3& cameraDirection, ShaderAction* inArray, UInt32* outIndexArray, UInt32 count)
	{
		Float32 squareDists[MAX_SECONDPASS_DRAW_LIST];
		for (UInt32 i = 0; i < count; i++)
		{
			Matrix4x4 worldTransform;
			inArray[i].getShaderMatVar("modelMat", worldTransform);
			squareDists[i] = (worldTransform.getPos() - cameraPosition).lengthSquare();
			outIndexArray[i] = i;
		}

		quickSortBlend(squareDists, outIndexArray, 0, count - 1);
	}

}