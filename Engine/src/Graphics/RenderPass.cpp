#include "RenderPass.h"
#include "GfxMgr.h"

void RenderPass::PreRender()
{
    std::sort(mRenderables.begin(), mRenderables.end(),
              [](Renderable* lhs, Renderable* rhs) { return lhs->transform.pos.z < rhs->transform.pos.z; });
}

void RenderPass::Execute()
{
    PreRender();

    mFramebuffer->BindFramebuffer();

    const glm::mat4 view_cam = GfxMgr->mCam.GetViewMtx();
    const glm::mat4 proj_cam = GfxMgr->mCam.GetProjMtx();

    if (mShader)
    {
        Shader* shader = mShader->get();
        shader->Bind();
        shader->SetUniform("view", view_cam);
        shader->SetUniform("proj", proj_cam);

        std::for_each(mRenderables.begin(), mRenderables.end(),
                      [&shader](Renderable* comp)
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
                      [&proj_cam, &view_cam](Renderable* comp)
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
