/*
 *@file:   myglwidget.cpp
 *@author: 缪庆瑞
 *@date:   2016.10.10
 *@brief:  在该部件上实现3D绘图
 */
#include "myglwidget.h"
#include <GL/glu.h>
#include <QDebug>

//环境光，来自四面八方 四个参数RGBA 此处为半亮的环境光。倘若没有环境光，则未被漫射光照到的区域将无比黑暗
GLfloat lightAmbient[4] = {0.5,0.5,0.5,1.0};
//漫射光由特定光源产生，直照的面会很亮，未被照射到的区域将是黑暗的 四个参数RGBA 此处为全亮的漫射光
GLfloat lightDiffuse[4] = {1.0,1.0,1.0,1.0};
//保存漫射光源的位置 前三个参数是在XYZ轴上的位移 最后一个1.0表示此处就是光源的位置
//此处光源位置始终正对正面，随着正面的旋转光源也跟着旋转
GLfloat lightPosition[4] = {0.0,0.0,2.0,1.0};
//保存雾的颜色
GLfloat fogColor[4] = {0.5,0.5,0.5,1.0};

MyGLWidget::MyGLWidget(QWidget *parent) :
    QGLWidget(parent)
{
    this->setWindowTitle("3D绘图");
    depth=-6.0;//z轴往里的深度，越深图形越小
    xRot=0.0;//绕轴旋转的角度
    yRot=0.0;
    zRot=0.0;
    speed=10.0;//绕轴旋转的速度

    light=false;//初始不使用光源
    blend=false;//初始不使用融合
    fog=false;//初始不使用雾

    //显示列表的变量初始化
    box=0;
    top=0;

}
/*
 *@author:  缪庆瑞
 *@date:    2016.10.12
 *@brief:   绘制平面图形并着色旋转
 */
void MyGLWidget::paintPlaneGL()
{
    glLoadIdentity();//重置模型视图矩阵 将当前点移动到窗口中心 xyz轴的中心
    //绘制三角形
    glTranslatef(-2.0,0.0,depth);//相对当前点（0,0,0）移动坐标原点 x左移2.0 z默认里移6.0
    glRotatef(yRot,0.0,1.0,0.0);//旋转，参数1表示角度，后三个参数决定旋转方向
    glBegin(GL_TRIANGLES);//绘制三角形 以3个点为一组
    /*设置顶点颜色，给不同的顶点设置不同颜色，最终整个图形颜色将会混合，即平滑着色。如果所有顶点
     *都共用一种颜色，即产生单调着色*/
    glColor3f(1.0,0.0,0.0);
    glVertex3f(0.0,1.0,0.0);//以移动后的坐标原点绘制顶点(上) 三角形默认逆时针
    glColor3f(0.0,1.0,0.0);
    glVertex3f(-1.0,-1.0,0.0);
    glColor3f(0.0,0.0,1.0);
    glVertex3f(1.0,-1.0,0.0);
    glEnd();//绘制结束
    //因为之前旋转了，坐标轴可能变化了，不重置可能会出现意料外的结果
    glLoadIdentity();//重置模型视图矩阵 将当前点移动到窗口中心 xyz轴的中心
    //绘制四边形 以4个点为一组
    glTranslatef(2.0,0.0,depth);//相对当前点右移2.0
    glRotatef(xRot,1.0,0.0,0.0);//旋转，参数1表示角度，后三个参数决定旋转方向
    glBegin(GL_QUADS);//绘制四边形
    glColor3f(1.0,1.0,0.0);
    glVertex3f(-1.0,1.0,0.0);//以移动后的坐标原点绘制顶点(左上) 四边形默认逆时针
    glColor3f(0.0,1.0,1.0);
    glVertex3f(-1.0,-1.0,0.0);
    glColor3f(1.0,0.0,1.0);
    glVertex3f(1.0,-1.0,0.0);
    glColor3f(1.0,0.5,0.5);
    glVertex3f(1.0,1.0,0.0);
    glEnd();
    //glBegin(GL_POLYGON);//多边形 可以任意个顶点
}
/*
 *@author:  缪庆瑞
 *@date:    2016.10.12
 *@brief:   绘制金字塔(四棱锥)，并设置旋转
 */
void MyGLWidget::paintPyramidGL()
{
    glLoadIdentity();
    //绘制金字塔(四棱锥) 以底面中心为坐标原点
    glTranslatef(0.0,-1.0,depth);//相对当前点（0,0,0）移动坐标原点 下移 里移
    glRotatef(xRot,1.0,0.0,0.0);//饶轴旋转
    glRotatef(yRot,0.0,1.0,0.0);
    glRotatef(zRot,0.0,0.0,1.0);
    glBegin(GL_TRIANGLES);//绘制三角形 以3个点为一组构成一个面
    //前侧面
    glColor3f(1.0,0.0,0.0);//设置顶点颜色各个面交点的颜色保持一致  所有面均保持逆时针画
    glVertex3f(0.0,2.0,0.0);
    glColor3f(0.0,1.0,0.0);
    glVertex3f(-1.0,0.0,1.0);
    glColor3f(0.0,0.0,1.0);
    glVertex3f(1.0,0.0,1.0);
    //右侧面
    glColor3f(1.0,0.0,0.0);
    glVertex3f(0.0,2.0,0.0);//上顶点与前侧面顶点一致
    glColor3f(0.0,0.0,1.0);
    glVertex3f(1.0,0.0,1.0);//左下顶点与前侧面右下顶点一致
    glColor3f(0.0,1.0,0.0);
    glVertex3f(1.0,0.0,-1.0);//右下顶点与左下顶点关于z轴对称
    //左侧面
    glColor3f(1.0,0.0,0.0);
    glVertex3f(0.0,2.0,0.0);//上顶点与前侧面顶点一致
    glColor3f(0.0,0.0,1.0);
    glVertex3f(-1.0,0.0,-1.0);//左下顶点与右下顶点关于z轴对称
    glColor3f(0.0,1.0,0.0);
    glVertex3f(-1.0,0.0,1.0);//右下顶点与前侧面左下顶点一致
    //后侧面
    glColor3f(1.0,0.0,0.0);
    glVertex3f(0.0,2.0,0.0);//上顶点与前侧面顶点一致
    glColor3f(0.0,1.0,0.0);
    glVertex3f(1.0,0.0,-1.0);
    glColor3f(0.0,0.0,1.0);
    glVertex3f(-1.0,0.0,-1.0);
    glEnd();
}
/*
 *@author:  缪庆瑞
 *@date:    2016.10.12
 *@brief:   绘制立方体，并设置旋转
 */
void MyGLWidget::paintCubeGL()
{
    glLoadIdentity();
    //绘制立方体 以立方体的中心为原点
    glTranslatef(0.0,0.0,depth);//相对当前点（0,0,0）移动坐标原点 向里移6.0
    //glRotatef(45,0.0,1.0,0.0);//对坐标轴进行旋转 右手定则 第一个参数为旋转角度 后三个确定旋转轴向量
    glRotatef(xRot,1.0,0.0,0.0);//绕x轴正方向旋转 下类似 旋转过程中坐标轴由初始状态正对的面确定
    glRotatef(yRot,0.0,1.0,0.0);
    glRotatef(zRot,0.0,0.0,1.0);
    glBegin(GL_QUADS);//绘制四边形 各顶点的坐标推荐使用对称原理求出
    //上面
    glColor3f(1.0,0.0,0.0);//在下一次调用该方法前，各顶点均使用该颜色
    glVertex3f(1.0,1.0,1.0);
    glVertex3f(1.0,1.0,-1.0);
    glVertex3f(-1.0,1.0,-1.0);
    glVertex3f(-1.0,1.0,1.0);
    //下面
    glColor3f(0.0,1.0,0.0);
    glVertex3f(1.0,-1.0,1.0);
    glVertex3f(1.0,-1.0,-1.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glVertex3f(-1.0,-1.0,1.0);
    //前面
    glColor3f(0.0,0.0,1.0);
    glVertex3f(1.0,1.0,1.0);
    glVertex3f(-1.0,1.0,1.0);
    glVertex3f(-1.0,-1.0,1.0);
    glVertex3f(1.0,-1.0,1.0);
    //后面
    glColor3f(1.0,0.0,1.0);//紫色
    glVertex3f(1.0,1.0,-1.0);
    glVertex3f(-1.0,1.0,-1.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glVertex3f(1.0,-1.0,-1.0);
    //左边
    glColor3f(0.0,1.0,1.0);//天蓝
    glVertex3f(-1.0,1.0,-1.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glVertex3f(-1.0,-1.0,1.0);
    glVertex3f(-1.0,1.0,1.0);
    //右边
    glColor3f(1.0,1.0,0.0);//黄色
    glVertex3f(1.0,1.0,-1.0);
    glVertex3f(1.0,-1.0,-1.0);
    glVertex3f(1.0,-1.0,1.0);
    glVertex3f(1.0,1.0,1.0);
    glEnd();
}
/*
 *@author:  缪庆瑞
 *@date:    2016.10.12
 *@brief:   绘制六棱柱，并设置旋转
 */
void MyGLWidget::paintPrism6GL()
{
    glLoadIdentity();
    //绘制六棱柱 以柱体中心为原点
    glTranslatef(0.0,0.0,depth);//相对当前点（0,0,0）移动坐标原点 向里移6.0
    //glRotatef(45,0.0,1.0,0.0);//对坐标轴进行旋转 右手定则 第一个参数为旋转角度 后三个确定旋转轴向量
    glRotatef(xRot,1.0,0.0,0.0);//绕x轴正方向旋转 下类似 旋转过程中坐标轴由初始状态正对的面确定
    glRotatef(yRot,0.0,1.0,0.0);
    glRotatef(zRot,0.0,0.0,1.0);
    glBegin(GL_QUADS);//绘制四边形 各顶点的坐标推荐使用对称原理求出
    //前面
    glColor3f(1.0,0.0,0.0);
    glVertex3f(1.0,2.0,1.7);
    glVertex3f(1.0,-2.0,1.7);
    glVertex3f(-1.0,-2.0,1.7);
    glVertex3f(-1.0,2.0,1.7);
    //后面
    glColor3f(0.0,1.0,0.0);
    glVertex3f(1.0,2.0,-1.7);
    glVertex3f(1.0,-2.0,-1.7);
    glVertex3f(-1.0,-2.0,-1.7);
    glVertex3f(-1.0,2.0,-1.7);
    //左前
    glColor3f(0.0,0.0,1.0);
    glVertex3f(-1.0,2.0,1.7);
    glVertex3f(-1.0,-2.0,1.7);
    glVertex3f(-2.0,-2.0,0.0);
    glVertex3f(-2.0,2.0,0.0);
    //右前
    glColor3f(1.0,1.0,0.0);//黄色
    glVertex3f(1.0,2.0,1.7);
    glVertex3f(2.0,2.0,0.0);
    glVertex3f(2.0,-2.0,0.0);
    glVertex3f(1.0,-2.0,1.7);
    //左后
    glColor3f(1.0,0.0,1.0);//紫色
    glVertex3f(-2.0,2.0,0.0);
    glVertex3f(-2.0,-2.0,0.0);
    glVertex3f(-1.0,-2.0,-1.7);
    glVertex3f(-1.0,2.0,-1.7);
    //右后
    glColor3f(0.0,1.0,1.0);//天蓝
    glVertex3f(1.0,2.0,-1.7);
    glVertex3f(1.0,-2.0,-1.7);
    glVertex3f(2.0,-2.0,0.0);
    glVertex3f(2.0,2.0,0.0);
    glEnd();
}
/*
 *@author:  缪庆瑞
 *@date:    2016.10.12
 *@brief:   绘制带纹理的立方体 绘制图形时顶点逆时针旋转 纹理显示正面 顺时针显示反面
 */
void MyGLWidget::paintTextureCube()
{
    glEnable(GL_TEXTURE_2D);//启用纹理 原本的颜色会变得暗淡 不启用则纹理图片不显示
    glLoadIdentity();
    //绘制立方体
    glTranslatef(0.0,0.0,depth);//相对当前点（0,0,0）移动坐标原点 向里移6.0
    //glRotatef(45,0.0,1.0,0.0);//对坐标轴进行旋转 右手定则 第一个参数为旋转角度 后三个确定旋转轴向量
    glRotatef(xRot,1.0,0.0,0.0);//绕x轴正方向旋转 下类似 旋转过程中坐标轴由初始状态正对的面确定
    glRotatef(yRot,0.0,1.0,0.0);
    glRotatef(zRot,0.0,0.0,1.0);
    //选择要使用的纹理图片 必须放在glBegin()之前或glEnd()之后
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glBegin(GL_QUADS);//绘制四边形
    //上面 每个面的顶点均逆时针旋转 以保证纹理贴图处于正面
    glColor3f(1.0,0.0,0.0);//颜色设置会使纹理图覆盖一层对应的色彩，不设置为正常图像颜色
    //参数1：纹理X坐标 0.0表示纹理左侧 0.5中点 1.0右侧 参数2：纹理Y坐标 0.0底部 0.5中点 1.0上部
    glTexCoord2f(0.0,1.0);//表示纹理图的左上顶点,与下一行设置的顶点位置对应
    glVertex3f(-1.0,1.0,-1.0);
    glTexCoord2f(0.0,0.0);
    glVertex3f(-1.0,1.0,1.0);
    glTexCoord2f(1.0,0.0);
    glVertex3f(1.0,1.0,1.0);
    glTexCoord2f(1.0,1.0);
    glVertex3f(1.0,1.0,-1.0);
    //下面
    glColor3f(0.0,1.0,0.0);
    glTexCoord2f(0.0,1.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glTexCoord2f(0.0,0.0);
    glVertex3f(-1.0,-1.0,1.0);
    glTexCoord2f(1.0,0.0);
    glVertex3f(1.0,-1.0,1.0);
    glTexCoord2f(1.0,1.0);
    glVertex3f(1.0,-1.0,-1.0);
    //前面
    glColor3f(1.0,1.0,1.0);//将颜色设成白色 否则会使用之前的绿色 是图片有点绿
    //参数1：纹理X坐标 0.0表示纹理左侧 0.5中点 1.0右侧 参数2：纹理Y坐标 0.0底部 0.5中点 1.0上部
    //纹理的顶点坐标要和图形坐标对应
    glTexCoord2f(0.0,1.0);
    glVertex3f(-1.0,1.0,1.0);
    glTexCoord2f(0.0,0.0);
    glVertex3f(-1.0,-1.0,1.0);
    glTexCoord2f(1.0,0.0);
    glVertex3f(1.0,-1.0,1.0);
    glTexCoord2f(1.0,1.0);
    glVertex3f(1.0,1.0,1.0);
    glEnd();//因为要使用第二张纹理图，使用纹理图的函数不能放在glbegin()和glEnd()中间

    glBindTexture(GL_TEXTURE_2D,texture[1]);//这里只是说绑定纹理图函数的用法，实际和上面共用一个纹理图
    glBegin(GL_QUADS);//绘制四边形
    //后面
    glColor3f(1.0,0.0,1.0);//紫色
    glTexCoord2f(0.0,1.0);
    glVertex3f(-1.0,1.0,-1.0);
    glTexCoord2f(0.0,0.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glTexCoord2f(1.0,0.0);
    glVertex3f(1.0,-1.0,-1.0);
    glTexCoord2f(1.0,1.0);
    glVertex3f(1.0,1.0,-1.0);
    //左边
    glColor3f(0.0,1.0,1.0);//天蓝
    glTexCoord2f(0.0,1.0);
    glVertex3f(-1.0,1.0,-1.0);
    glTexCoord2f(0.0,0.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glTexCoord2f(1.0,0.0);
    glVertex3f(-1.0,-1.0,1.0);
    glTexCoord2f(1.0,1.0);
    glVertex3f(-1.0,1.0,1.0);
    //右边
    glColor3f(1.0,1.0,0.0);//黄色
    glTexCoord2f(0.0,1.0);
    glVertex3f(1.0,1.0,-1.0);
    glTexCoord2f(0.0,0.0);
    glVertex3f(1.0,-1.0,-1.0);
    glTexCoord2f(1.0,0.0);
    glVertex3f(1.0,-1.0,1.0);
    glTexCoord2f(1.0,1.0);
    glVertex3f(1.0,1.0,1.0);
    glEnd();
}
/*
 *@author:  缪庆瑞
 *@date:    2016.10.17
 *@brief:   利用显示列表绘制许多相同的立方体，一次性在显示列表中建立好物体
 *          之后直接调用即可，大大降低CPU的使用
 */
void MyGLWidget::paintManyCubeGL()
{
    glEnable(GL_TEXTURE_2D);//启用纹理 原本的颜色会变得暗淡 不启用则纹理图片不显示
    glBindTexture(GL_TEXTURE_2D,texture[0]);//使用纹理图
    for(int i=1;i<6;i++)//层数
    {
        for(int j=0;j<i;j++)//每层立方体的个数
        {
            glLoadIdentity();//重置模型观察矩阵
            glTranslatef(1.4+(float(j)*2.8)-(float(i)*1.4),(6.0-(float(i)))*2.4-7.0,depth);
            glRotatef(45.0-(2.0 * i)+xRot,1.0,0.0,0.0);//每层绕x轴旋转的角度不一样
            glRotatef(45.0+yRot,0.0,1.0,0.0);//所有的立方体绕y轴旋转一致
            glColor3f(0.0,1.0,1.0);
            glCallList(box);//调用显示列表画立方体盒子
            glColor3f(0.8,0.6,0.0);
            glCallList(top);//调用显示列表画立方体顶面
        }
    }
}
/*
 *@author:  缪庆瑞
 *@date:    2016.10.12
 *@brief:   加载纹理图片，使用纹理主要就是给图形贴图，使产生的3D图形更真实好看。另外一个贴图所能表达的信息可能
 * 可以需要成百上千个多边形来实现，所以贴图也节省了cpu的时间。
 */
void MyGLWidget::loadGLTextures()
{
    QImage tex;
    QImage buf0;//用来临时存放加载的图片
    QImage buf1;
    QImage buf2;
    if(!buf0.load("./images/Koala.png"))
    {
        QMessageBox::information(this,"提示","加载图片失败！！");
        return;
    }
    if(!buf1.load("./images/Hydrangeas.png"))
    {
        QMessageBox::information(this,"提示","加载图片失败！！");
        return;
    }
    if(!buf2.load("./images/star.png"))
    {
        QMessageBox::information(this,"提示","加载图片失败！！");
        return;
    }
    tex=this->convertToGLFormat(buf0);//将图片转换成GL的格式 直接用的话图片的纹理顶点坐标与图形顶点不一定对应
    glGenTextures(3,texture);//声明创建3个纹理

    //创建第一个纹理，并存到指定位置
    glBindTexture(GL_TEXTURE_2D,texture[0]);//将纹理名字texture[0]绑定到纹理目标上 2D纹理只有高度和宽度
    /*真正的创建纹理 参数1：表示是2D纹理 参数2：图像的详细程度 参数3：数据的成分数红绿蓝三种
     *参数45：纹理的宽高  参数6：边框的大小 参数7：表示图像由RGBA组成 参数8：表示组成图像的数据为无符号
     *字节 参数9：表示纹理数据的来源*/
    //qDebug()<<tex.width()<<tex.height();
    glTexImage2D(GL_TEXTURE_2D,0,3,tex.width(),tex.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,tex.bits());
    //每创建一个纹理都需要设置放大或缩小的滤波方式，否则纹理显示不出来
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);//GL_LINEAR较消耗cpu 但可以保证平滑
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);//GL_NEAREST 放大会显斑驳

    //创建第二个纹理，并存到指定位置
    tex=this->convertToGLFormat(buf1);
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glTexImage2D(GL_TEXTURE_2D,0,3,tex.width(),tex.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,tex.bits());
    //放大或缩小纹理时采用的滤波方式 最后一个参数确定滤波方式 不使用此方法纹理显示不出来
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);//GL_LINEAR较消耗cpu 但可以保证平滑
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);//GL_NEAREST 放大会显斑驳 即低质量贴图

    //创建第三个纹理，并存到指定位置
    tex=this->convertToGLFormat(buf2);
    glBindTexture(GL_TEXTURE_2D,texture[2]);
    //使用mipmap对任意位图创建2D纹理 绕过对纹理宽高的限制
    gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,tex.width(),tex.height(),GL_RGBA,GL_UNSIGNED_BYTE,tex.bits());
    //放大或缩小纹理时采用的滤波方式 最后一个参数确定滤波方式 不使用此方法纹理显示不出来
    //第三个参数使用mipmap改变图形缩小时细节丢失情况 经测试有一种背景虚化的感觉
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);//GL_LINEAR较消耗cpu 但可以保证平滑
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);//GL_NEAREST 放大会显斑驳 即低质量贴图

}
/*
 *@author:  缪庆瑞
 *@date:    2016.10.10
 *@brief:   设置opengl的渲染环境，定义显示列表等，只在第一次调用resizeGL()或paintGL()前
 *          调用一次。
 */
void MyGLWidget::initializeGL()
{
    loadGLTextures();//载入纹理
    buildLists();//构建显示列表

    glClearColor(0.5,0.5,0.5,1.0);//设置清除屏幕时使用的颜色,四个参数分别表示rgba（0.0-1.0） 这里是灰色
    glShadeModel(GL_SMOOTH);//设置阴影平滑
    glClearDepth(1.0);//设置深度缓存 可以设想成屏幕后面的层
    glEnable(GL_DEPTH_TEST);//启用深度测试  记录图形在屏幕的深度值
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);//真正精细的透视修正 稍微影响性能 但透视图看起来更好

    //光源
    glLightfv(GL_LIGHT1,GL_AMBIENT,lightAmbient);//设置环境光 光源GL_LIGHT1 发光量在数组里
    glLightfv(GL_LIGHT1,GL_DIFFUSE,lightDiffuse);//设置漫射光 光源GL_LIGHT1 发光量在数组里
    glLightfv(GL_LIGHT1,GL_POSITION,lightPosition);//设置光源的位置
    glEnable(GL_LIGHT1);//启用1号光源
    /*在使能GL_LIGHTING(光源总控)之前，光源不会工作。启用光源后未被漫射光直射的面都相对暗淡
     *(取决于环境光的亮度)*/
    //glEnable(LIGHTING);//启用光源

    /*融合：用某个象素的颜色和已绘制在屏幕上各象素颜色相互结合，重点就是透明度的结合*/
    glColor4f(1.0,1.0,1.0,0.5);//全亮度 半透明绘制此物体
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);//设置融合类型

    //确定雾的类型 GL_EXP:简单渲染 GL_EXP2:略有提高，渲染全屏幕的雾 GL_LINEAR：最好的雾渲染
    glFogi(GL_FOG_MODE,GL_EXP2);
    //设定雾的颜色 灰色
    glFogfv(GL_FOG_COLOR,fogColor);
    //确定雾的密度
    glFogf(GL_FOG_DENSITY,0.35);
    //确定雾的渲染方式 GL_FOG_DENSITY：由opengl自己确定  GL_NICEST：按像素渲染，效果好 GL_FASTEST：按顶点渲染 速度快
    glHint(GL_FOG_HINT,GL_DONT_CARE);
    //确定雾能离屏幕的远近
    glFogf(GL_FOG_START,1.0);
    glFogf(GL_FOG_END,15.0);

}
/*
 *@author:  缪庆瑞
 *@date:    2016.10.10
 *@brief:   设置opengl的视图，投影等。每次改变部件大小都会调用该函数
 *          它使场景尺寸与窗口大小一致
 */
void MyGLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,(GLint)w,(GLint)h);//设置视图的大小
    glMatrixMode(GL_PROJECTION);//设置投影矩阵 为场景增加透视 使用下面的两行进行设置
    glLoadIdentity();//重置投影矩阵 将矩阵状态恢复成原始状态
    //设置透视投影矩阵，视角45° 纵横比为窗口纵横比 绘制深度的起点0.1 终点100
    gluPerspective(45.0,(GLfloat)w/(GLfloat)h,0.1,100.0);

    glMatrixMode(GL_MODELVIEW);//指明任何新的变换将会影响模型观察矩阵
    glLoadIdentity();
}
/*
 *@author:  缪庆瑞
 *@date:    2016.10.10
 *@brief:   3D图形的具体绘制函数，每当部件更新都会调用它
 */
void MyGLWidget::paintGL()
{
    //每次都要先清除屏幕，否则之前的图像存在无法实现动态变化。
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除屏幕和深度缓存 使用initializeGL()中设置的清屏颜色和深度缓存
    glLoadIdentity();//重置模型视图矩阵 将当前点移动到窗口中心 xyz轴的中心

    //paintPlaneGL();//1.绘制平面图形
    //paintPyramidGL();//2.绘制金字塔 四棱锥
    //paintCubeGL();//3.绘制立方体
    //paintPrism6GL();//4.绘制六棱柱
    paintTextureCube();//5.绘制带有纹理的立方体
    //paintManyCubeGL();//使用显示列表绘制多个相同的立方体






}
/*
 *@author:  缪庆瑞
 *@date:    2016.10.10
 *@brief:   键盘事件处理函数，对按下的键做出响应
 */
void MyGLWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_G:
        fog = !fog;//切换雾状态
        if(fog)
        {
            glEnable(GL_FOG);//开启雾
        }
        else
        {
            glDisable(GL_FOG);//关闭雾
        }
    case Qt::Key_L:
        light = !light;//切换光源状态
        if(light)
        {
            glEnable(GL_LIGHTING);//开启光源
        }
        else
        {
            glDisable(GL_LIGHTING);//关闭光源
        }
        break;
    case Qt::Key_B:
        blend = !blend;//切换融合状态
        if(blend)
        {
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);//开启融合后要关闭深度缓存
        }
        else
        {
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
        }
        break;
    case Qt::Key_X://绕X轴旋转角度
        xRot+=speed;
        break;
    case Qt::Key_Y://绕Y轴旋转角度
        yRot+=speed;
        break;
    case Qt::Key_Z://绕Z轴旋转角度
        zRot+=speed;
        break;
    case Qt::Key_Left://减慢旋转速度
        speed-=2;
        break;
    case Qt::Key_Right://加快旋转速度
        speed+=2;
        break;
    case Qt::Key_Up://增加深度，即视觉角度上图像离得更近
        if(depth<-1)
            depth+=1;
        break;
    case Qt::Key_Down://减小深度，即视觉角度上图像离得更远
        if(depth>-30)
            depth-=1;
        break;

    }
    updateGL();
    QGLWidget::keyPressEvent(event);
}
/*
 *@author:  缪庆瑞
 *@date:    2016.10.17
 *@brief:   建立显示列表，一次构建，之后可以数次调用该函数中创建的显示列表
 */
void MyGLWidget::buildLists()
{
    box=glGenLists(2);//建立两个显示列表的空间，并返回第一个的指针
    //glNewList()和glEndList()之间构造显示列表 显示列表只会被构建一次
    glNewList(box,GL_COMPILE);//开始构造显示列表，存放到box所指空间 第二个参数表示预先在内存中构造列表
    glBegin(GL_QUADS);//四边形

    glNormal3f(0.0,-1.0,0.0);//指定法向量 底面
    glTexCoord2f(1.0,1.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glTexCoord2f(1.0,0.0);
    glVertex3f(-1.0,-1.0,1.0);
    glTexCoord2f(0.0,0.0);
    glVertex3f(1.0,-1.0,1.0);
    glTexCoord2f(0.0,1.0);
    glVertex3f(1.0,-1.0,-1.0);

    glNormal3f(0.0,0.0,1.0);//指定法向量 前面
    glTexCoord2f(0.0,1.0);
    glVertex3f(-1.0,1.0,1.0);
    glTexCoord2f(0.0,0.0);
    glVertex3f(-1.0,-1.0,1.0);
    glTexCoord2f(1.0,0.0);
    glVertex3f(1.0,-1.0,1.0);
    glTexCoord2f(1.0,1.0);
    glVertex3f(1.0,1.0,1.0);

    glNormal3f(0.0,0.0,-1.0);//指定法向量 后面
    glTexCoord2f(1.0,1.0);
    glVertex3f(-1.0,1.0,-1.0);
    glTexCoord2f(1.0,0.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glTexCoord2f(0.0,0.0);
    glVertex3f(1.0,-1.0,-1.0);
    glTexCoord2f(0.0,1.0);
    glVertex3f(1.0,1.0,-1.0);

    glNormal3f(1.0,0.0,0.0);//指定法向量 右侧面
    glTexCoord2f(1.0,1.0);
    glVertex3f(1.0,1.0,-1.0);
    glTexCoord2f(0.0,1.0);
    glVertex3f(1.0,1.0,1.0);
    glTexCoord2f(0.0,0.0);
    glVertex3f(1.0,-1.0,1.0);
    glTexCoord2f(1.0,0.0);
    glVertex3f(1.0,-1.0,-1.0);

    glNormal3f(1.0,0.0,0.0);//指定法向量 左侧面
    glTexCoord2f(0.0,1.0);
    glVertex3f(-1.0,1.0,-1.0);
    glTexCoord2f(0.0,0.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glTexCoord2f(1.0,0.0);
    glVertex3f(-1.0,-1.0,1.0);
    glTexCoord2f(1.0,1.0);
    glVertex3f(-1.0,1.0,1.0);
    glEnd();

    glEndList();//告诉opengl 第一个显示列表建立完毕 存放立方体的盒子

    top=box+1;//定义第二个显示列表
    glNewList(top,GL_COMPILE);
    glBegin(GL_QUADS);

    glNormal3f(0.0,1.0,0.0);//指定法向量 上面
    glTexCoord2f(0.0,1.0);//表示纹理图的左上顶点,与下一行设置的顶点位置对应
    glVertex3f(-1.0,1.0,-1.0);
    glTexCoord2f(0.0,0.0);
    glVertex3f(-1.0,1.0,1.0);
    glTexCoord2f(1.0,0.0);
    glVertex3f(1.0,1.0,1.0);
    glTexCoord2f(1.0,1.0);
    glVertex3f(1.0,1.0,-1.0);

    glEnd();
    glEndList();//存放立方体的顶部
}

