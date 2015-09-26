# MD2FrameAnimation
MD2 Vertex Animation with Direct3D 基于MD2文件的简单顶点序列帧动画原理和实例。
/*
Author:  JeromeCen
Date:  2015.5.26
Description: Direct3D 3D序列帧动画原理和实例
*/
一、运行环境和建议：
本项目目前是只有D3D的版本，但应用到Opengl环境中也是很简单的。
1.运行环境：
1)Direct3D SDK
2)VS2008以上版本
3)直接打开.sln运行即可，有编译不过的可以联系我，Email: JeromeCen@hotmail.com,qq:1021900404
使用：编译成功后，按enter键即可实现不同动作的切换。

2.建议：
对MD2格式文件不熟悉的读者，先阅读：
https://linux.ucla.edu/~phaethon/q3a/formats/md2-schoenblum.html
http://www.cppblog.com/liangairan/articles/60607.html
两篇文章。再阅读本代码。

需要更深入理解：可以研究一下鬼火引擎代码，本项目即参考了鬼火引擎实现的简单版本：
http://irrlicht.sourceforge.net/ 

二、MD2文件格式的理解：
md2文件结构的静态模型动画数据
1.帧和顶点：其实md2文件中，帧记录的是关键帧，顶点数是一帧的顶点数，顶点数据是存储在帧结构体信息中的顶点指针内存中。
2.帧和顶点关系:关键帧中的第几帧信息，其中一帧是记录了一帧的信息，主要是记录了真正的顶点压缩后的数据，对顶点的压缩比例，平移
，和顶点法向量索引信息。
3.三角形和glcmd索引：顶点数据和纹理数据，三角形网格数据索引顶点和纹理数据，如果用了glcmd那么在glcmd中存储纹理坐标和顶点索引。
glcommand是一帧中的三角形索引，格式是(都是int类型count代表了两层含义三角形类型和绘制的顶点组个数）：
count1s11t11vertexIndex11 s12t12vertexIndex12...s1nt1nvertexIndex1n
count2s21t21vertexIndex21 s22t22vertexIndex22...s2nt2nvertexIndex2n
...
countNsN1tN1vertexIndexN1 sN2tN2vertexIndexN2...sNntNnvertexIndexNn
\0
4.绘制时候，可以用triangle_t三角形索引顶点坐标和纹理坐标，也可以用gl command来索引顶点坐标和纹理坐标。其中
triangle_t和gl command都是记录了一帧的信息，需要在遍历关键帧数里面填充顶点和纹理信息。

三、实现的时候遇到的一些问题和解决方法：
1.无论是当前帧的，还是关键帧的：顶点，索引，法向量，纹理坐标，颜色的大小都是 三角形大小乘以3.
顶点缓存，纹理坐标缓存只是用于三角形索引得到值用的，索引缓存顺序就好了。

2.动画插值错了，新的插值方式？
见Animate函数和Interpolate()函数，动画插值的是用简单的线性插值：Xinterpolated = Xinital + InterpolationPercent * (Xfinal - Xinital)插值的驱动是时间，当时间超过关键帧的FPS时候，那么进行关键帧递进；当关键帧内先进行频繁的插值但只有当前插值增加幅度大于动画真实的FPS需要的插值幅度时候，才会进行插值计算。

3.目前是用lefthand左手坐标系的方式来加载，顶点法向量也没有做特别的处理，世界坐标转换设置为单位矩阵，观察变换，和投影变换也是简单的方式，具体见代码。后续看是否需要特别的调整，mesh网格数据层面的（z轴）；摄像机位置会引起的x轴改变; 叉乘方向不同导致的法向量不同，旋转不同导致四元数或者矩阵不同; 渲染时候的背面剔除和顶点索引顺序。

4.纹理坐标错了，导致纹理插值不正确, 读取和数据类型问题?
就是因为读取数据类型是short，但是转换为标准纹理坐标时候全部需要float来存储，是因为类型转换精度丢失出现了问题。


