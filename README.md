> # QuantisationAndCompression
算法实现图片的有损压缩和无损压缩以及解压缩


### 背景

有损量化这里介绍从8-8-8到5-5-5和5-6-5的量化压缩原理及其编程实现。无损压缩这里基于游长编码算法(利用像素的重复)首先提出一种简单改进算法，即在图像的各通道上进行游长编码，利用各通道像素值得重复性分别进行压缩，一定程度上提高了压缩性，因为两个相邻像素虽然不同，但他们的某个通道可能会相同，这种方法简单高效，但适应性差，主要利用了图像的空间冗余性；之后，提出压缩前的分块处理，为了减少图像各区域之间的巨大差异造成的重复性被分割削弱，先从二维上将图像分块，再对分块进行空间冗余压缩，也就是更加充分地利用图像的空间冗余性。

> Giuthub源码：[https://github.com/jiangxh1992/QuantisationAndCompression](https://github.com/jiangxh1992/QuantisationAndCompression)

> English Version:[http://jiangxh.top/articles/2016-10/compressionEN](http://jiangxh.top/articles/2016-10/compressionEN)

***

### 有损量化5-5-5和5-6-5

压缩对象使图像的RGB通道值，每个值都是0~255之间的数字，分别使用8位保存，因此原始图像每个像素要使用3*8=24位，即‘8-8-8’。这里要将其量化压缩，使用16位来保存24位的信息，因此要损失部分精度,压缩率固定为1.50。

![这里写图片描述](http://img.blog.csdn.net/20161116114543300)
![这里写图片描述](http://img.blog.csdn.net/20161116114552537)
![这里写图片描述](http://img.blog.csdn.net/20161116114604819)

**5-5-5**指的是只使用低15位，剩下的一位弃用，这样每个通道一致的都压缩为5位；

**5-6-5**则是充分使用了16位，其中G通道占6位，另外两通道各占5位。

***算法原理很简单：***

压缩时5-5-5是将每个通道的二进制值都右移3位（除以8），保留剩下的5位，然后依次放入16位数的低15位；解压时分别将各通道的5位二进制数取出并左移3位，低位补0还原成8位，因此低三位的数据丢失掉了。

5-6-6和5-5-5同理，只是G通道的二进制数右移2两位（除以4），将剩下的6位和其他两通道的10位一同放入16位二进制数中。解压时同样是低位补0还原为8位。

***算法代码：***

程序背景说明：`width`和`height`指的是导入的图片的尺寸（像素个数），`Input`是保存三个通道的像素值的数组，这里windows工程存储的三通道顺序为B,G,R，不是R,G,B。

![这里写图片描述](http://img.blog.csdn.net/20161115223337876)

**5-5-5：**

```cpp
unsigned char *CAppQuantize::Quantize555(int &qDataSize) {

	int i, j ;
	unsigned int r, g, b ;
	unsigned short rgb16 ;

	qDataSize = width * height * 2 ;

	unsigned char *quantizedImageData = new unsigned char[width * height * 2] ;

	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
			b = pInput[(i + j * width) * 3 + 0] ;	// Blue Color Component
			g = pInput[(i + j * width) * 3 + 1] ;	// Red Color Component
			r = pInput[(i + j * width) * 3 + 2] ;	// Green COlor Component
			rgb16 = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3) ;
			quantizedImageData[(i + j * width) * 2 + 0] = rgb16 & 0xFF ;
			quantizedImageData[(i + j * width) * 2 + 1] = (rgb16 >> 8) & 0xFF ;
		}
	}

	return quantizedImageData ;
}
```

```cpp
void CAppQuantize::Dequantize555(unsigned char *quantizedImageData, unsigned char *unquantizedImageData) {

	int i, j ;
	unsigned int r, g, b ;
	unsigned short rgb16 ;

	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
			rgb16 = quantizedImageData[(i + j * width) * 2 + 0] | (((unsigned short) quantizedImageData[(i + j * width) * 2 + 1]) << 8) ;
			b = rgb16 & 0x1F;
			g = (rgb16 >> 5) & 0x1F ;
			r = (rgb16 >> 10) & 0x1F ;
			unquantizedImageData[(i + j * width) * 3 + 0] = (b << 3) ;
			unquantizedImageData[(i + j * width) * 3 + 1] = (g << 3) ;
			unquantizedImageData[(i + j * width) * 3 + 2] = (r << 3) ;
		}
	}
}
```

**5-6-5:**

```cpp
unsigned char *CAppQuantize::Quantize565(int &qDataSize) {

	int i, j;
	unsigned int r, g, b;
	unsigned short rgb16;

	qDataSize = width * height * 2 ;
	unsigned char *quantizedImageData = new unsigned char[width * height * 2] ;

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			b = pInput[(i + j * width) * 3 + 0];	// Blue Color Component
			g = pInput[(i + j * width) * 3 + 1];	// Green Color Component
			r = pInput[(i + j * width) * 3 + 2];	// Red Color Component
			rgb16 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3); // r分量和b分量右移3位，g分量右移2位

			quantizedImageData[(i + j * width) * 2 + 0] = rgb16 & 0xFF; // 高8位
			quantizedImageData[(i + j * width) * 2 + 1] = (rgb16 >> 8) & 0xFF;// 低8位
		}
	}
	
	return quantizedImageData ;
}
```

```cpp
void CAppQuantize::Dequantize565(unsigned char *quantizedImageData, unsigned char *unquantizedImageData) {

	int i, j;
	unsigned int r, g, b;
	unsigned short rgb16;

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			rgb16 = quantizedImageData[(i + j * width) * 2 + 0] | (((unsigned short)quantizedImageData[(i + j * width) * 2 + 1]) << 8);
			b = rgb16 & 0x1F;   // 保留高5位
			g = (rgb16 >> 5) & 0x3F;// 右移5位后保留高6位
			r = (rgb16 >> 11) & 0x1F;// 右移11位后保留高5位
			unquantizedImageData[(i + j * width) * 3 + 0] = (b << 3); // 左移3位，高位补0
			unquantizedImageData[(i + j * width) * 3 + 1] = (g << 2); // 左移2位，高位补0
			unquantizedImageData[(i + j * width) * 3 + 2] = (r << 3); // 左移3位，高位补0
		}
	}
}
```

***

### 通道游长编码无损压缩

![这里写图片描述](http://img.blog.csdn.net/20161115230431749)

***压缩过程：***

压缩后的数据形式是：两个无符号8位二进制数为一组，第一个存储重复的个数，第二个存储通道值。

分B,G,R三个通道依次进行，对于每个通道从第一个值开始，计算后面相同的值的个数，碰到新的不同值或者重复个数超出了8位数的表示上限，则将之前的重复值和通道值保存到一组压缩后的数据中，并开始下一组同样的计算压缩，直到所有数据全部压缩完。

***解压过程：***

解压也是分三个通道依次解压，由于三个通道的压缩数据都放在了同一个数组，因此先要找到G通道和R通道的开始位置offset_g和offset_r，寻找方法是循环同时累加计算前面通道各像素的重复个数，每当重复个数达到图片像素个数，下一个即时另一个通道的开始了。之后开始解压，每次从各通道取一个值组成一个像素，直到各通道同时取完，解压后的数据就是压缩前的原数据了，实现了图像的无损压缩。

***算法代码：***

**无损压缩：**

```cpp
unsigned char *CAppCompress::Compress(int &cDataSize) {

	unsigned char *compressedData ;
	cDataSize = width * height * 3 ;	

	// 存储压缩后的数据,最差的情况尺寸也不会到大于cDataSize * 2
	compressedData = new unsigned char[cDataSize * 2];
	// 实际压缩字符长度
	int compressedSize = 0;

	// 采用分通道游离的方法，按照每个通道相邻像素的重复性进行压缩
	// 1.b通道
	unsigned short curB = pInput[0];// 第一个像素的b
	unsigned short repeat = 1;// 重复次数
	for (int i = 1; i < cDataSize / 3; i++)
	{
		unsigned short nextB = pInput[i * 3 + 0];// 下一个像素的b
		if (nextB == curB && repeat < 127)
		{
			++repeat;
			// 如果是最后一个则存储
			if (i == cDataSize / 3 - 1)
			{
				// 存储最后一个b值组
				compressedData[compressedSize] = repeat;
				compressedData[compressedSize + 1] = curB;
				// 增加编码数据长度
				compressedSize += 2;
			}
		}
		else
		{
			// 存储上一个b值组
			compressedData[compressedSize] = repeat;
			compressedData[compressedSize + 1] = curB;
			// 增加编码数据长度
			compressedSize += 2;
			// 换下一种b值
			curB = nextB;
			repeat = 1;
			// 如果是最后一个
			if (i == cDataSize / 3 - 1)
			{
				// 存储最后一个b值
				compressedData[compressedSize] = 1;
				compressedData[compressedSize + 1] = curB;
				// 增加编码数据长度
				compressedSize += 2;
			}
		}
	}

	// 2.g通道
	unsigned short curG = pInput[1];// 第一个像素的g
	repeat = 1;// 重复次数
	for (int i = 1; i < cDataSize / 3; i++)
	{
		unsigned short nextG = pInput[i * 3 + 1];// 下一个像素的g
		if (nextG == curG && repeat <= 127)
		{
			++repeat;
			// 如果是最后一个则存储
			if (i == cDataSize / 3 - 1)
			{
				// 存储最后一个g值组
				compressedData[compressedSize] = repeat;
				compressedData[compressedSize + 1] = curG;
				// 增加编码数据长度
				compressedSize += 2;
			}
		}
		else
		{
			// 存储上一个g值组
			compressedData[compressedSize] = repeat;
			compressedData[compressedSize + 1] = curG;
			// 增加编码数据长度
			compressedSize += 2;
			// 换下一种g值
			curG = nextG;
			repeat = 1;
			// 如果是最后一个
			if (i == cDataSize / 3 - 1)
			{
				// 存储最后一个g值
				compressedData[compressedSize] = 1;
				compressedData[compressedSize + 1] = curB;
				// 增加编码数据长度
				compressedSize += 2;
			}
		}
	}

	// 3.r通道
	unsigned short curR = pInput[2];// 第一个像素的r
	repeat = 1;// 重复次数
	for (int i = 1; i < cDataSize / 3; i++)
	{
		unsigned short nextR = pInput[i * 3 + 2];// 下一个像素的r
		if (nextR == curR && repeat <= 127)
		{
			++repeat;
			// 如果是最后一个则存储
			if (i == cDataSize / 3 - 1)
			{
				// 存储最后一个g值组
				compressedData[compressedSize] = repeat;
				compressedData[compressedSize + 1] = curR;
				// 增加编码数据长度
				compressedSize += 2;
			}
		}
		else
		{
			// 存储上一个g值组
			compressedData[compressedSize] = repeat;
			compressedData[compressedSize + 1] = curR;
			// 增加编码数据长度
			compressedSize += 2;
			// 换下一种r值
			curR = nextR;
			repeat = 1;
			// 如果是最后一个
			if (i == cDataSize / 3 - 1)
			{
				// 存储最后一个r值
				compressedData[compressedSize] = 1;
				compressedData[compressedSize + 1] = curR;
				// 增加编码数据长度
				compressedSize += 2;
			}
		}
	}

	// 取出压缩后的纯数据
	cDataSize = compressedSize;
	unsigned char *finalData = new unsigned char[cDataSize];
	for (int i = 0; i < cDataSize; i++)
	{
		unsigned char temp = compressedData[i];
		finalData[i] = temp;
	}
	delete compressedData;
	compressedData = finalData;

	return compressedData;
}
```

**无损解压缩：**

```cpp
void CAppCompress::Decompress(unsigned char *compressedData, int cDataSize, unsigned char *uncompressedData) {
	
	// 寻找g通道和r通道在压缩数据数组中的偏移坐标
	int offset_r = 0, offset_g = 0;
	int pixelCount = 0;
	for (int i = 0; i < cDataSize;)
	{
		int curRpeat = compressedData[i];
		pixelCount += curRpeat;
		i += 2;
		if (pixelCount == width*height)
		{
			offset_g = i;// g通道的开始坐标
		}
		if (pixelCount == width*height * 2)
		{
			offset_r = i;// r通道的开始坐标
		}
	}

	unsigned int b, g, r;
	int repeat;
	// 1.还原b通道
	for (int i = 0, j = 0; i < width*height, j < offset_g; j += 2)
	{
		// 恢复一组重复的b值
		repeat = compressedData[j];
		for (int p = 0; p < repeat; p++)
		{
			int d = compressedData[j + 1];
			uncompressedData[i * 3 + p*3 + 0] = compressedData[j + 1];
		}
		i += repeat;
	}

	// 2.还原g通道
	for (int i = 0, j = offset_g; i < width*height, j < offset_r; j += 2)
	{
		repeat = compressedData[j];
		for (int p = 0; p < repeat; p++)
		{
			int d = compressedData[j + 1];
			uncompressedData[i * 3 + p * 3 + 1] = compressedData[j + 1];
		}
		i += repeat;
	}

	// 3.还原r通道
	for (int i = 0, j = offset_r; i < width*height, j < cDataSize; j += 2)
	{
		repeat = compressedData[j];
		for (int p = 0; p < repeat; p++)
		{
			int d = compressedData[j + 1];
			uncompressedData[i * 3 + p * 3 + 2] = compressedData[j + 1];
		}
		i += repeat;
	}
}
```

***效果分析：***

**最好情况：** 算法基于通道像素重复，最好的情况自然是纯色推图像。算法对于颜色比较单调的图像压缩效果较好；

**最差情况：** 最差情况是三个通道相邻的两个像素的值都不同，这时候压缩后的数据刚好是原数据的两倍大小，每一个像素各通道值都额外用了一个8位存储重复个数，且重复个数都是1。

压缩到六十四分之一：
![这里写图片描述](http://img.blog.csdn.net/20161115231304487)

压缩到三分之一：
![这里写图片描述](http://img.blog.csdn.net/20161115231059969)

压缩失败：
![这里写图片描述](http://img.blog.csdn.net/20161115231246847)


***

### 空间分割优化

算法步骤：首先先后对图像进行横向、纵向或者横向、纵向扫描，扫描时对每一行或者每一列计算平均值，当平均值和上一行或者列差值大于阈值时，设置当前行列为一个边界。例如：如果先横向分割，后纵向分割，那么横向分割后将图像分成了几个子图像，之后再对每一个子图像进行同样的纵向分割，即可将图像分成内部类似的子图像区域。之后再对子图像进行空间冗余性压缩。图像分割效果大致如下：
![这里写图片描述](http://img.blog.csdn.net/20161126182243009)


**示例代码：**

```cpp
type cpp

```

### 量化压缩与无损压缩组合

直接使用该算法对图像压缩，面对色彩变化丰富的图像总是压缩失败的，但如果先对图像进行有损量化，再对量化后的图像进行无损压缩往往可以取得不错的效果。量化实际上是为无损压缩提高了容错性，本来两个通道值相差可能很小，如果能包容这微小的差异那么将大大提高压缩率。下图中打印的三个压缩率依次是：直接压缩的压缩率、有损量化的压缩率、对量化后的图像再进行无损压缩的压缩率。

![这里写图片描述](http://img.blog.csdn.net/20161115231112017)
![这里写图片描述](http://img.blog.csdn.net/20161115231134126)
![这里写图片描述](http://img.blog.csdn.net/20161115231205158)
![这里写图片描述](http://img.blog.csdn.net/20161115231234908)

### 进一步的先进压缩算法

实际中的图像往往是颜色丰富错综复杂的，仅仅利用空间冗余来进行压缩适应性太低，利用重复性进行游长编码压缩往往不但压缩失败，甚至会使压缩后的图像体积更大，最差的情况如上所说会是原来的两倍。因此为了研究适应性更好的算法就要从更多维度去利用图像本身的重复性（图像的重复性再多维度上是很大的）。

从另一种程度上图像信息是一种信号信息，图像数据的内在联系不仅仅是相邻像素之间的相似性而已，图像可以向声音信号一样常使用波信号去模拟预测，挖掘图像整体的信息后可以利用已有信息在压缩过程中对未压缩数据进行预测，利用图像的多维度重复性进行进一步的压缩。

#### 自适应预测熵编码

。。。

#### 基于分片的无损压缩方法

。。。
