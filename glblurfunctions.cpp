#include "glblurfunctions.h"

static const Vertex sg_vertexes[] =
{
  Vertex( QVector3D( 1.0f,  1.0f, 1.0f)),
  Vertex( QVector3D(-1.0f,  1.0f, 1.0f)),
  Vertex( QVector3D(-1.0f, -1.0f, 1.0f)),
  Vertex( QVector3D( 1.0f, -1.0f, 1.0f))
};

GLBlurFunctions::GLBlurFunctions()
{
    m_Context = new QOpenGLContext();
    m_Context->setFormat(QSurfaceFormat::defaultFormat());
    m_Context->create();

    m_Surface = new QOffscreenSurface();
    m_Surface->create();
    m_Surface->setFormat(m_Context->format());

    m_Context->makeCurrent(m_Surface);

    initializeOpenGLFunctions();

    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

    m_ShaderProgram_kawase_up = new QOpenGLShaderProgram();
    m_ShaderProgram_kawase_up->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
    m_ShaderProgram_kawase_up->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/dual_kawase_up.frag");
    m_ShaderProgram_kawase_up->link();

    m_ShaderProgram_kawase_down = new QOpenGLShaderProgram();
    m_ShaderProgram_kawase_down->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
    m_ShaderProgram_kawase_down->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/dual_kawase_down.frag");
    m_ShaderProgram_kawase_down->link();

    m_Buffer.create();
    m_Buffer.bind();
    m_Buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_Buffer.allocate(sg_vertexes, sizeof(sg_vertexes));

    m_VertexArrayObject.create();
    m_VertexArrayObject.bind();

    m_ShaderProgram_kawase_up->enableAttributeArray(0);
    m_ShaderProgram_kawase_up->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());

    m_ShaderProgram_kawase_down->enableAttributeArray(0);
    m_ShaderProgram_kawase_down->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());

    m_Texture = new QOpenGLTexture(QImage());
}

GLBlurFunctions::~GLBlurFunctions()
{
    delete m_ShaderProgram_kawase_up;
    delete m_ShaderProgram_kawase_down;

    for (int i = 0; i < m_FBO_vector.size(); i++)
    {
        delete m_FBO_vector[i];
    }

    delete m_Texture;
}

QImage GLBlurFunctions::blurImage_kawase(QImage imageToBlur, int offset, int iterations)
{
    for (int i = 0; i < m_FBO_vector.size(); i++)
    {
        delete m_FBO_vector[i];
    }

    m_FBO_vector.clear();
    m_FBO_vector.append(new QOpenGLFramebufferObject(imageToBlur.size(), QOpenGLFramebufferObject::CombinedDepthStencil, GL_TEXTURE_2D));

    for (int i = 1; i <= iterations; i++)
    {
        m_FBO_vector.append(new QOpenGLFramebufferObject(m_FBO_vector[i - 1]->size() / 2, QOpenGLFramebufferObject::CombinedDepthStencil, GL_TEXTURE_2D));

        QOpenGLFunctions::glBindTexture(GL_TEXTURE_2D, m_FBO_vector.last()->texture());
        QOpenGLFunctions::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        QOpenGLFunctions::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    //Creating m_Texture takes too much time. TODO: fix this
    m_Texture = new QOpenGLTexture(imageToBlur.mirrored());
    m_Texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_Texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

    m_ShaderProgram_kawase_down->setUniformValue("offset", QVector2D(1 + offset, 1 + offset));
    m_ShaderProgram_kawase_up->setUniformValue("offset", QVector2D(1 + offset, 1 + offset));

    //Initial downsample    
    renderToFBO(m_FBO_vector[1], m_Texture, m_ShaderProgram_kawase_down);
    delete m_Texture;

    //Downsample
    for (int i = 2; i <= iterations; i++)
    {
        renderToFBO(m_FBO_vector[i], m_FBO_vector[i - 1]->texture(), m_ShaderProgram_kawase_down);
    }

    //Upsample
    for (int i = iterations - 1; i >= 0; i--)
    {
        renderToFBO(m_FBO_vector[i], m_FBO_vector[i + 1]->texture(), m_ShaderProgram_kawase_up);
    }

    return m_FBO_vector[0]->toImage();
}

void GLBlurFunctions::renderToFBO(QOpenGLFramebufferObject *targetFBO, QOpenGLTexture *sourceTexture, QOpenGLShaderProgram *shader)
{
    targetFBO->bind();
    sourceTexture->bind();
    shader->bind();

    shader->setUniformValue("iResolution", QVector2D(targetFBO->size().width(), targetFBO->size().height()));

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(sg_vertexes) / sizeof(sg_vertexes[0]));
}

void GLBlurFunctions::renderToFBO(QOpenGLFramebufferObject *targetFBO, GLuint sourceTexture, QOpenGLShaderProgram *shader)
{
    targetFBO->bind();
    QOpenGLFunctions::glBindTexture(GL_TEXTURE_2D, sourceTexture);
    shader->bind();

    shader->setUniformValue("iResolution", QVector2D(targetFBO->size().width(), targetFBO->size().height()));

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(sg_vertexes) / sizeof(sg_vertexes[0]));
}
