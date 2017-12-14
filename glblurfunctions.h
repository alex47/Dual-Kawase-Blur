#ifndef GLBLURFUNCTIONS_H
#define GLBLURFUNCTIONS_H

#include <QDebug>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QtMath>
#include <QMatrix4x4>

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLContext>
#include <QOffscreenSurface>

#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include <QVector2D>

#include "vertex.h"


class GLBlurFunctions : protected QOpenGLFunctions
{
public:
    GLBlurFunctions();
    ~GLBlurFunctions();

    QImage blurImage_kawase(QImage imageToBlur, int offset, int iterations);

private:
    void renderToFBO(QOpenGLFramebufferObject* targetFBO, QOpenGLTexture *sourceTexture, QOpenGLShaderProgram *shader);
    void renderToFBO(QOpenGLFramebufferObject* targetFBO, GLuint sourceTexture, QOpenGLShaderProgram *shader);

    QOpenGLVertexArrayObject m_VertexArrayObject;
    QOpenGLShaderProgram *m_ShaderProgram_kawase_up;
    QOpenGLShaderProgram *m_ShaderProgram_kawase_down;

    QVector<QOpenGLFramebufferObject*> m_FBO_vector;
    QOpenGLBuffer m_Buffer;
    QOpenGLTexture *m_Texture;

    QOffscreenSurface *m_Surface;
    QOpenGLContext *m_Context;
};

#endif // GLBLURFUNCTIONS_H
