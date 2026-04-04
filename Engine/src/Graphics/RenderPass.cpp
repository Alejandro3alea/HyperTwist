#include "RenderPass.h"
#include "Graphics/GfxMgr.h"

void RenderPass::PreRender()
{
    std::sort(mRenderables.begin(), mRenderables.end(),
              [](IRenderable* lhs, IRenderable* rhs) { return lhs->transform.pos.z < rhs->transform.pos.z; });

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::uvec2 bufSize = mFramebuffer->BufferSize();
    glViewport(0, 0, bufSize.x, bufSize.y);
}

void RenderPass::Execute()
{
    mFramebuffer->BindFramebuffer();

    PreRender();

    const glm::mat4 view_cam = GfxMgr->mCam.GetViewMtx();
    const glm::mat4 proj_cam = GfxMgr->mCam.GetProjMtx();

    if (mShader)
    {
        Shader* shader = mShader->get();
        shader->Bind();
        shader->SetUniform("view", view_cam);
        shader->SetUniform("proj", proj_cam);

        std::for_each(mRenderables.begin(), mRenderables.end(),
                      [&shader](IRenderable* comp)
                      {
                          if (comp->mShader && comp->mbIsVisible)
                          {
                              const Transform t = comp->mParentTransform.has_value()
                                                      ? comp->transform + comp->mParentTransform.value()
                                                      : comp->transform;
                              glm::mat4 model = t.GetModelMtx();
                              shader->SetUniform("model", model);
                              comp->Render();
                          }
                      });
    }
    else
    {
        std::for_each(mRenderables.begin(), mRenderables.end(),
                      [&proj_cam, &view_cam](IRenderable* comp)
                      {
                          if (comp->mShader && comp->mbIsVisible)
                          {
                              Shader* shader = comp->mShader->get();
                              shader->Bind();

                              const Transform t = comp->mParentTransform.has_value()
                                                      ? comp->transform + comp->mParentTransform.value()
                                                      : comp->transform;
                              glm::mat4 model = t.GetModelMtx();
                              shader->SetUniform("model", model);
                              shader->SetUniform("view", view_cam);
                              shader->SetUniform("proj", proj_cam);
                              comp->Render();
                          }
                      });
    }

    mFramebuffer->UnbindFramebuffer();
}
