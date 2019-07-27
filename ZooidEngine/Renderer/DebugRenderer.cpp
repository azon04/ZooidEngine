#include "DebugRenderer.h"

#include "Memory/Handle.h"
#include "BufferData.h"

#include "ResourceManagers/BufferManager.h"
#include "Renderer/IGPUBufferArray.h"

#include "Events/Events.h"

#include "Renderer/IRenderer.h"
#include "ZEGameContext.h"
#include "Renderer/DrawList.h"
#include "ResourceManagers/ShaderManager.h"
#include "Scene/TextComponent.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(DebugRenderer, Component)

	DebugRenderer::DebugRenderer(GameContext* gameContext) 
		: Component(gameContext)
	{
		m_lineBuffers.reset(2);
	}

	void DebugRenderer::Init(GameContext* gameContext)
	{
		ScopedRenderThreadOwnership renderLock(gameContext->getRenderer());

		Handle hDebugRenderer("Debug Renderer", sizeof(DebugRenderer));
		s_instance = new(hDebugRenderer) DebugRenderer(gameContext);

		Handle hBufferData("Buffer Data", sizeof(BufferData));
		s_instance->m_lineBufferData = new(hBufferData) BufferData(VERTEX_BUFFER);
		s_instance->m_lineBufferData->setBufferLayout( BUFFER_LAYOUT_V3_C3);
		s_instance->m_lineBufferData->SetData(&s_instance->m_lineBuffers[0], sizeof(DebugPointStruct), s_instance->m_lineBuffers.size());
		s_instance->m_lineBufferData->setStaticBuffer(false);
		s_instance->m_lineGPUBufferData = BufferManager::getInstance()->createGPUBufferFromBuffer(s_instance->m_lineBufferData, false);
		
		Handle hBufferArray = BufferManager::getInstance()->createBufferArray(s_instance->m_lineGPUBufferData, nullptr, nullptr);
		s_instance->m_lineBufferArray = hBufferArray.getObject<IGPUBufferArray>();

		s_instance->m_currentIndex = 0;
		s_instance->m_currentTextIndex = 0;

		s_instance->setupSphere();
		s_instance->setupCube();
		s_instance->setupCone();

		s_instance->setupComponent();
	}

	void DebugRenderer::Destroy()
	{

	}

	void DebugRenderer::DrawMatrixBasis(Matrix4x4& mats)
	{
		s_instance->drawMatrixBasis(mats);
	}

	void DebugRenderer::DrawLine(const Vector3& p1, const Vector3& p2, const Vector3& color)
	{
		s_instance->drawLine(p1, p2, color);
	}

	void DebugRenderer::DrawTextScreen(const char* text, const Vector2& _position, const Vector3& _color, Float32 _scale)
	{
		s_instance->drawScreenText(text, _position, _color, _scale);
	}

	void DebugRenderer::DrawTextWorld(const char* text, Matrix4x4& _transform)
	{
		s_instance->drawWorldText(text, _transform);
	}

	void DebugRenderer::DrawDebugSphere(const Vector3& position, Float32 radius)
	{
		Matrix4x4 mat;
		mat.fromScaleAndPosition(radius, radius, radius, position);
		s_instance->m_debugSphereTransforms.push_back(mat);
	}

	void DebugRenderer::DrawDebugBox(const Vector3& extend, const Vector3& centerOffset, Matrix4x4& tranform)
	{
		Matrix4x4 matExtend;
		matExtend.setScale(extend);
		matExtend.setPos(centerOffset);

		matExtend = matExtend * tranform;

		s_instance->drawDebugCube(matExtend);
	}

	void DebugRenderer::DrawDebugCone(const Vector3& top, float distance, float radius, Matrix4x4& transform)
	{
		Matrix4x4 mat;
		mat.setScale(Vector3(radius, distance, radius));
		mat.setPos(top);

		mat = mat * transform;

		s_instance->drawDebugCone(mat);
	}

	void DebugRenderer::setupComponent()
	{
		Component::setupComponent();

		addEventDelegate(Event_GATHER_RENDER, &DebugRenderer::handleGatherRender);
	}

	void DebugRenderer::handleGatherRender(Event* _event)
	{
		if (m_currentIndex > 0)
		{
			MeshRenderInfo* renderInfo = m_gameContext->getDrawList()->m_debugMeshRenderGatherer.nextRenderInfo();
			renderInfo->m_shaderChain = ShaderManager::GetInstance()->getShaderChain(Z_SHADER_CHAIN_3D_DEFAULT_COLOR_LINE);
			renderInfo->m_gpuBufferArray = m_lineBufferArray;
			renderInfo->m_renderTopology = ERenderTopologyEnum::TOPOLOGY_LINE;
			renderInfo->drawCount = m_currentIndex;
			renderInfo->m_worldTransform = Matrix4x4();

			// Update Line data
			m_lineBufferData->SetData(&s_instance->m_lineBuffers[0], sizeof(DebugPointStruct), s_instance->m_lineBuffers.size());
			
			{
				ScopedRenderThreadOwnership renderLock(m_gameContext->getRenderer());
				m_lineGPUBufferData->refresh();
			}

		}

		while (m_currentTextIndex < m_textComponents.size())
		{
			m_textComponents[m_currentTextIndex++]->setVisible(false);
		}

		// Reset
		m_currentIndex = 0;
		m_currentTextIndex = 0;

		for (int i = 0; i < m_debugSphereTransforms.size(); i++)
		{
			MeshRenderInfo* renderInfo = m_gameContext->getDrawList()->m_debugMeshRenderGatherer.nextRenderInfo();
			renderInfo->m_shaderChain = ShaderManager::GetInstance()->getShaderChain(Z_SHADER_CHAIN_3D_DEFAULT_COLOR_LINE);
			renderInfo->m_gpuBufferArray = m_sphereGPUBufferArrray;
			renderInfo->m_renderTopology = ERenderTopologyEnum::TOPOLOGY_LINE;
			renderInfo->drawCount = m_sphereGPUBufferArrray->getDataCount();
			renderInfo->m_worldTransform = m_debugSphereTransforms[i];
		}


		for (int i = 0; i < m_debugCubeTransforms.size(); i++)
		{
			MeshRenderInfo* renderInfo = m_gameContext->getDrawList()->m_debugMeshRenderGatherer.nextRenderInfo();
			renderInfo->m_shaderChain = ShaderManager::GetInstance()->getShaderChain(Z_SHADER_CHAIN_3D_DEFAULT_COLOR_LINE);
			renderInfo->m_gpuBufferArray = m_cubeGPUBufferArray;
			renderInfo->m_renderTopology = ERenderTopologyEnum::TOPOLOGY_LINE;
			renderInfo->drawCount = m_cubeGPUBufferArray->getDataCount();
			renderInfo->m_worldTransform = m_debugCubeTransforms[i];
		}

		for (int i = 0; i < m_debugConeTransforms.size(); i++)
		{
			MeshRenderInfo* renderInfo = m_gameContext->getDrawList()->m_debugMeshRenderGatherer.nextRenderInfo();
			renderInfo->m_shaderChain = ShaderManager::GetInstance()->getShaderChain(Z_SHADER_CHAIN_3D_DEFAULT_COLOR_LINE);
			renderInfo->m_gpuBufferArray = m_coneGPUBufferArray;
			renderInfo->m_renderTopology = ERenderTopologyEnum::TOPOLOGY_LINE;
			renderInfo->drawCount = m_coneGPUBufferArray->getDataCount();
			renderInfo->m_worldTransform = m_debugConeTransforms[i];
		}

		m_debugConeTransforms.clear();
		m_debugSphereTransforms.clear();
		m_debugCubeTransforms.clear();
	}

	void DebugRenderer::drawMatrixBasis(Matrix4x4& mat)
	{
		Vector3& pos = mat.getPos();
		drawLine(pos, pos + mat.getU(), Vector3(1.0f, 0.0f, 0.0f));
		drawLine(pos, pos + mat.getV(), Vector3(0.0f, 1.0f, 0.0f));
		drawLine(pos, pos + mat.getN(), Vector3(0.0f, 0.0f, 1.0f));
	}

	void DebugRenderer::drawLine(const Vector3& p1, const Vector3& p2, const Vector3& color)
	{
		if (m_currentIndex >= m_lineBuffers.size())
		{
			m_lineBuffers.push_back(DebugPointStruct());
			m_lineBuffers.push_back(DebugPointStruct());
		}

		DebugPointStruct& p1Struct = m_lineBuffers[m_currentIndex++];
		p1Struct.Point = p1;
		p1Struct.Color = color;

		DebugPointStruct& p2Struct = m_lineBuffers[m_currentIndex++];
		p2Struct.Point = p2;
		p2Struct.Color = color;
	}

	void DebugRenderer::drawScreenText(const char* text, const Vector2& _position, const Vector3& _color, Float32 _scale)
	{
		if (m_currentTextIndex >= m_textComponents.size())
		{
			Handle hTextComponent("Text Component", sizeof(TextComponent));
			m_textComponents.push_back(new(hTextComponent) TextComponent(m_gameContext));

			addChild(m_textComponents[m_currentTextIndex]);
			m_textComponents[m_currentTextIndex]->setupComponent();
		}

		TextComponent* textComponent = m_textComponents[m_currentTextIndex++];
		textComponent->setVisible(true);
		textComponent->setText(text);
		textComponent->setWorldPosition(Vector3(_position.getX(), _position.getY(), 0.0f));
		textComponent->setColor(_color);
		textComponent->setScale(Vector3(_scale, _scale, 1.0f));
		textComponent->setDrawSpace(DRAW_SCREEN);
	}

	void DebugRenderer::drawWorldText(const char* text, Matrix4x4& _transform)
	{
		if(m_currentTextIndex >= m_textComponents.size())
		{
			Handle hTextComponent("Text Component", sizeof(TextComponent));
			m_textComponents.push_back(new(hTextComponent) TextComponent(m_gameContext));

			addChild(m_textComponents[m_currentTextIndex]);
			m_textComponents[m_currentTextIndex]->setupComponent();
		}

		TextComponent* textComponent = m_textComponents[m_currentTextIndex++];
		textComponent->setVisible(true);
		textComponent->setText(text);
		textComponent->setColor(Vector3(1.0f, 1.0f, 1.0f));
		textComponent->setWorldTransform(_transform);
		textComponent->setDrawSpace(DRAW_WORLD_SPACE);
	}

	void DebugRenderer::drawDebugSphere(Matrix4x4& _transform)
	{
		m_debugSphereTransforms.push_back(_transform);
	}

	void DebugRenderer::drawDebugCube(Matrix4x4& _transform)
	{
		m_debugCubeTransforms.push_back(_transform);
	}

	void DebugRenderer::drawDebugCone(Matrix4x4& _transform)
	{
		m_debugConeTransforms.push_back(_transform);
	}

	void DebugRenderer::setupSphere()
	{
		Array<DebugPointStruct> points;

		Float32 countPerRing = 16;
		Float32 angleStep = DegToRad(360.0f / countPerRing);

		// Draw Point On XY Plane
		Float32 pointAngle = 0.0f;
		DebugPointStruct prevPoint;
		prevPoint.Point.m_x = cosf(pointAngle);
		prevPoint.Point.m_y = sinf(pointAngle);
		prevPoint.Point.m_z = 0.0f;
		prevPoint.Color = Vector3(1.0f, 0.0f, 0.0f);

		for (int i = 1; i <= countPerRing + 1; i++)
		{
			DebugPointStruct point;
			point.Point.m_x = cosf(pointAngle);
			point.Point.m_y = sinf(pointAngle);
			point.Point.m_z = 0.0f;
			point.Color = Vector3(1.0f, 0.0f, 0.0f);

			points.push_back(prevPoint);
			points.push_back(point);

			prevPoint = point;

			pointAngle += angleStep;
		}

		// Draw On YZ plane
		pointAngle = 0.0f;

		prevPoint.Point.m_y = cosf(pointAngle);
		prevPoint.Point.m_z = sinf(pointAngle);
		prevPoint.Point.m_x = 0.0f;
		prevPoint.Color = Vector3(1.0f, 0.0f, 0.0f);

		for (int i = 1; i <= countPerRing + 1; i++)
		{
			DebugPointStruct point;
			point.Point.m_y = cosf(pointAngle);
			point.Point.m_z = sinf(pointAngle);
			point.Point.m_x = 0.0f;
			point.Color = Vector3(1.0f, 0.0f, 0.0f);

			points.push_back(prevPoint);
			points.push_back(point);

			prevPoint = point;

			pointAngle += angleStep;
		}

		// Draw On ZX plane
		pointAngle = 0.0f;

		prevPoint.Point.m_z = cosf(pointAngle);
		prevPoint.Point.m_x = sinf(pointAngle);
		prevPoint.Point.m_y = 0.0f;
		prevPoint.Color = Vector3(1.0f, 0.0f, 0.0f);

		for (int i = 0; i <= countPerRing + 1; i++)
		{
			DebugPointStruct point;
			point.Point.m_z = cosf(pointAngle);
			point.Point.m_x = sinf(pointAngle);
			point.Point.m_y = 0.0f;
			point.Color = Vector3(1.0f, 0.0f, 0.0f);
			
			points.push_back(prevPoint);
			points.push_back(point);

			prevPoint = point;

			pointAngle += angleStep;
		}

		{
			BufferData sphereData(VERTEX_BUFFER);
			sphereData.SetData(points.data(), sizeof(DebugPointStruct), points.size());
			sphereData.setBufferLayout(BUFFER_LAYOUT_V3_C3);

			ScopedRenderThreadOwnership renderOwnership(gGameContext->getRenderer());
			IGPUBufferData* vertexBuffer = BufferManager::getInstance()->createGPUBufferFromBuffer(&sphereData);
			m_sphereGPUBufferArrray = BufferManager::getInstance()->createBufferArray(vertexBuffer, nullptr, nullptr).getObject<IGPUBufferArray>();
		}
	}

	void DebugRenderer::setupCube()
	{
		Array<DebugPointStruct> points;
		
		Vector3 color(0.0f, 0.0f, 1.0f);

		points.push_back({ Vector3(1.0f, 1.0f, 1.0f), color });
		points.push_back({ Vector3(1.0f, 1.0f, -1.0f), color });

		points.push_back({ Vector3(1.0f, -1.0f, 1.0f), color });
		points.push_back({ Vector3(1.0f, -1.0f, -1.0f), color });

		points.push_back({ Vector3(-1.0f, 1.0f, 1.0f), color });
		points.push_back({ Vector3(-1.0f, 1.0f, -1.0f), color });

		points.push_back({ Vector3(-1.0f, -1.0f, 1.0f), color });
		points.push_back({ Vector3(-1.0f, -1.0f, -1.0f), color });

		points.push_back({ Vector3(1.0f, -1.0f, 1.0f), color });
		points.push_back({ Vector3(-1.0f, -1.0f, 1.0f), color });

		points.push_back({ Vector3(1.0f, 1.0f, 1.0f), color });
		points.push_back({ Vector3(-1.0f, 1.0f, 1.0f), color });

		points.push_back({ Vector3(1.0f, -1.0f, -1.0f), color });
		points.push_back({ Vector3(-1.0f, -1.0f, -1.0f), color });

		points.push_back({ Vector3(1.0f, 1.0f, -1.0f), color });
		points.push_back({ Vector3(-1.0f, 1.0f, -1.0f), color });

		points.push_back({ Vector3(1.0f, 1.0f, 1.0f), color });
		points.push_back({ Vector3(1.0f, -1.0f, 1.0f), color });

		points.push_back({ Vector3(1.0f, 1.0f, -1.0f), color });
		points.push_back({ Vector3(1.0f, -1.0f, -1.0f), color });

		points.push_back({ Vector3(-1.0f, 1.0f, 1.0f), color });
		points.push_back({ Vector3(-1.0f, -1.0f, 1.0f), color });

		points.push_back({ Vector3(-1.0f, 1.0f, -1.0f), color });
		points.push_back({ Vector3(-1.0f, -1.0f, -1.0f), color });

		{
			BufferData cubeData(VERTEX_BUFFER);
			cubeData.SetData(points.data(), sizeof(DebugPointStruct), points.size());
			cubeData.setBufferLayout(BUFFER_LAYOUT_V3_C3);

			ScopedRenderThreadOwnership renderOwnership(gGameContext->getRenderer());
			IGPUBufferData* vertexBuffer = BufferManager::getInstance()->createGPUBufferFromBuffer(&cubeData);
			m_cubeGPUBufferArray = BufferManager::getInstance()->createBufferArray(vertexBuffer, nullptr, nullptr).getObject<IGPUBufferArray>();
		}
	}

	void DebugRenderer::setupCone()
	{
		Array<DebugPointStruct> points;

		Vector3 color(1.0f, 0.0f, 1.0f);

		// four lines on top
		points.push_back({ Vector3(0.0f, 0.0f, 0.0f), color });
		points.push_back({ Vector3(0.0f, 1.0f, 1.0f), color });

		points.push_back({ Vector3(0.0f, 0.0f, 0.0f), color });
		points.push_back({ Vector3(0.0f, 1.0f, -1.0f), color });

		points.push_back({ Vector3(0.0f, 0.0f, 0.0f), color });
		points.push_back({ Vector3(1.0f, 1.0f, 0.0f), color });

		points.push_back({ Vector3(0.0f, 0.0f, 0.0f), color });
		points.push_back({ Vector3(-1.0f, 1.0f, 0.0f), color });

		Float32 countPerRing = 16;
		Float32 angleStep = DegToRad(360.0f / countPerRing);

		Float32 pointAngle = 0.0f;
		DebugPointStruct prevPoint;

		prevPoint.Point.m_z = cosf(pointAngle);
		prevPoint.Point.m_x = sinf(pointAngle);
		prevPoint.Point.m_y = 1.0f;
		prevPoint.Color = color;

		for (int i = 0; i <= countPerRing + 1; i++)
		{
			DebugPointStruct point;
			point.Point.m_z = cosf(pointAngle);
			point.Point.m_x = sinf(pointAngle);
			point.Point.m_y = 1.0f;
			point.Color = color;

			points.push_back(prevPoint);
			points.push_back(point);

			prevPoint = point;

			pointAngle += angleStep;
		}

		{
			BufferData cubeData(VERTEX_BUFFER);
			cubeData.SetData(points.data(), sizeof(DebugPointStruct), points.size());
			cubeData.setBufferLayout(BUFFER_LAYOUT_V3_C3);

			ScopedRenderThreadOwnership renderOwnership(gGameContext->getRenderer());
			IGPUBufferData* vertexBuffer = BufferManager::getInstance()->createGPUBufferFromBuffer(&cubeData);
			m_coneGPUBufferArray = BufferManager::getInstance()->createBufferArray(vertexBuffer, nullptr, nullptr).getObject<IGPUBufferArray>();
		}
	}

	ZE::DebugRenderer* DebugRenderer::s_instance = nullptr;

}