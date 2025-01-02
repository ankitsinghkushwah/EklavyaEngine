//
//  DebugHelper.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 25/06/2023.
//

#include "DebugRenderer.hpp"
#include "ShaderProgram.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "AssetManager.h"
#include "TransformComponent.h"
#include "Frustum.h"

using namespace Eklavya::Renderer;
using namespace Eklavya::Asset;

DebugRenderer::DebugRenderer()
{
	glm::vec3 min, max;
	mUnlitSolids = AssetManager::GetInstance().GetAsset<ShaderProgram>("unlit_solids");
	VertexArrayObject::SetupVAOForBox(mBoxVAO, min, max);
	VertexArrayObject::SetupVAOForSphere(mSphereVAO, min, max);
}

DebugRenderer::~DebugRenderer()
{
	mBoxVAO.Destroy();
	mSphereVAO.Destroy();
}

void DebugRenderer::SetData(glm::mat4& projection, glm::mat4& view)
{
	mProjection = projection;
	mView = view;
}

glm::mat4 DebugRenderer::GetModel(glm::vec3 t, const glm::quat& r, glm::vec3 s)
{
	glm::mat4 rotation = glm::mat4_cast(r);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), s);
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), t);

	return translation * rotation * scale;
}

void DebugRenderer::DrawLine(glm::vec3 start, glm::vec3 end, glm::vec4 color, float thickness, bool disableDepthTest)
{
	if (disableDepthTest)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);
	}
	glm::vec3 direction = glm::normalize(end - start);
	float     len = glm::length(end - start);
	glm::quat q = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f));
	q = glm::normalize(q);
	glm::vec3 translate = start + ((end - start) / 2.0f);
	glm::vec3 scale(thickness, thickness, len);
	glm::mat4 model = GetModel(translate, q, scale);

	mUnlitSolids->Use();
	mUnlitSolids->SetMat4("projection", mProjection);
	mUnlitSolids->SetMat4("view", mView);
	mUnlitSolids->SetMat4("model", model);
	mUnlitSolids->SetVec3("color", glm::vec3(color));
	mUnlitSolids->SetFloat("opacity", color.a);
	mBoxVAO.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	mBoxVAO.Unbind();

	if (disableDepthTest)
	{
		glDepthFunc(GL_LEQUAL);
	}
}

void DebugRenderer::DrawSphere(glm::vec3 center, float radius, glm::vec4 color)
{
	glm::vec3 rotation;
	glm::mat4 model = GetModel(center, glm::quat(rotation), glm::vec3(radius));

	mUnlitSolids->Use();
	mUnlitSolids->SetMat4("projection", mProjection);
	mUnlitSolids->SetMat4("view", mView);
	mUnlitSolids->SetMat4("model", model);
	mUnlitSolids->SetVec3("color", glm::vec3(color));
	mUnlitSolids->SetFloat("opacity", color.a);
	mSphereVAO.Bind();
	glDrawElements(GL_TRIANGLES, mSphereVAO.GetIndicesSize(), GL_UNSIGNED_INT, 0);
	mSphereVAO.Unbind();
}

void DebugRenderer::DrawSphere(glm::vec3 center, glm::vec3 extents, glm::vec4 color)
{
	glm::vec3 rotation;
	glm::mat4 model = GetModel(center, glm::quat(rotation), extents);

	mUnlitSolids->Use();
	mUnlitSolids->SetMat4("projection", mProjection);
	mUnlitSolids->SetMat4("view", mView);
	mUnlitSolids->SetMat4("model", model);
	mUnlitSolids->SetVec3("color", glm::vec3(color));
	mUnlitSolids->SetFloat("opacity", color.a);
	mSphereVAO.Bind();
	glDrawElements(GL_TRIANGLES, mSphereVAO.GetIndicesSize(), GL_UNSIGNED_INT, 0);
	mSphereVAO.Unbind();
}

void DebugRenderer::DrawBox(glm::vec3 center, glm::vec3 rotation, glm::vec3 extents, glm::vec4 color)
{
	glm::mat4 model = GetModel(center, glm::quat(rotation), extents);

	mUnlitSolids->Use();
	mUnlitSolids->SetMat4("projection", mProjection);
	mUnlitSolids->SetMat4("view", mView);
	mUnlitSolids->SetMat4("model", model);
	mUnlitSolids->SetVec3("color", glm::vec3(color));
	mUnlitSolids->SetFloat("opacity", color.a);
	mBoxVAO.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	mBoxVAO.Unbind();
}

void DebugRenderer::DrawBound(const glm::mat4& model, const Bound& bound, glm::vec4 color)
{
	glm::mat4 scaledModel = glm::scale(model, bound.Extents());
	mUnlitSolids->Use();
	mUnlitSolids->SetMat4("projection", mProjection);
	mUnlitSolids->SetMat4("view", mView);
	mUnlitSolids->SetMat4("model", scaledModel);
	mUnlitSolids->SetVec3("color", glm::vec3(color));
	mUnlitSolids->SetFloat("opacity", color.a);
	mBoxVAO.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	mBoxVAO.Unbind();
}

void DebugRenderer::DrawFrustum(const Frustum& frustum)
{
	std::vector<std::array<int, 2>> planeIndices = {// front
	                                                {0, 1},
	                                                {1, 2},
	                                                {2, 3},
	                                                {3, 0},

	                                                // back
	                                                {4, 5},
	                                                {5, 6},
	                                                {6, 7},
	                                                {7, 4},

	                                                // middle
	                                                {0, 4},
	                                                {1, 5},
	                                                {2, 6},
	                                                {3, 7}};

	const glm::vec3* points = frustum.Points();

	for (const std::array<int, 2>& indices : planeIndices)
	{
		glm::vec3 s = points[indices[0]];
		glm::vec3 e = points[indices[1]];
		DrawLine(s, e, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 3.0f);
	}
}

void DebugRenderer::DrawPoints(std::vector<glm::vec3>& points, glm::vec4 color)
{
}

#define RGB(r, g, b) glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f)

glm::vec4 R = RGB(255, 100, 100);
glm::vec4 G = RGB(38, 244, 124);
glm::vec4 B = RGB(5, 195, 244);

float extend = 3.0f;
float thickness = 3.0f;

void DebugRenderer::DrawTransform(TransformComponent& transform)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	glm::vec3 pos = transform.Position();
	glm::vec3 right = pos + transform.right() * extend;
	glm::vec3 forward = pos + transform.forward() * extend;
	glm::vec3 up = pos + transform.up() * extend;

	// X
	DrawLine(pos, right, R, thickness);

	// Y
	DrawLine(pos, up, G, thickness);

	// Z
	DrawLine(pos, forward, B, thickness);

	glDepthFunc(GL_LEQUAL);
}

void DebugRenderer::AddLine(glm::vec3 start, glm::vec3 end, glm::vec4 color, float thickness)
{
	mLinesToDraw.emplace_back(start, end, color, thickness);
}

void DebugRenderer::AddSphere(glm::vec3 center, float radius, glm::vec4 color)

{
	mSpheresToDraw.emplace_back(center, color, radius);
}

void DebugRenderer::DrawAddedShapes()
{
	for (DebugLine& l : mLinesToDraw)
		DrawLine(l.start, l.end, l.color, l.thickness);
	for (DebugSphere& s : mSpheresToDraw)
		DrawSphere(s.center, s.radius, s.color);
}
