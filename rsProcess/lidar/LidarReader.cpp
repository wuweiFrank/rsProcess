#include"LidarReader.h"
#include"..\AuxiliaryFunction.h"

//打开LAS文件读取LAS文件的头文件和变长字段文件
long LASLidarReader::LidarReader_Open(const char* pathLidar)
{
	if (m_lasFile != NULL)
		fclose(m_lasFile);
	errno_t err;
	err = fopen_s(&m_lasFile, pathLidar, "rb");
	if (err != 0)
		return -1;

	//读取头文件
	m_lasHeader.LASHeader_Read(m_lasFile);
	if (m_lasHeader.number_of_variable_length_records == 0)
	{
		//没有变长字段
	}
	else
	{
		//读取变长字段
		m_lasvarHeader = new LASVariableRecord[m_lasHeader.number_of_variable_length_records];
		for (int i = 0; i < m_lasHeader.number_of_variable_length_records; ++i)
		{
			m_lasvarHeader[i].LASVariableRecord_Read(m_lasFile);
		}
	}
	m_isDatasetOpen = true;
	return 0;
}

//读取LAS文件，给的参数为是否将所有数据读入内存和读取数据采样间隔
long LASLidarReader::LidarReader_Read(bool inMemory, int readSkip/*=1*/)
{
	if (!m_isDatasetOpen)
	{
		printf("do not open file\n");
		exit(-1);
	}
	fseek(m_lasFile, m_lasHeader.offset_to_point_data, SEEK_SET);
	int totalLasNumber = 0;
	long long  memoryUsed = 0;
	totalLasNumber = m_lasHeader.number_of_point_records / readSkip;
	memoryUsed = totalLasNumber*m_lasHeader.point_data_record_length;
	if (inMemory&&memoryUsed > LargestMemoryToRead)
	{
		printf("too large memory to allocate\n");
		printf("advice to upper samples\n");
		return -1;
	}
	int *pointsRect			= NULL;
	unsigned char* readOnce = NULL;
	if (m_Progress != NULL)
		m_Progress->SetStepCount(m_lasHeader.number_of_point_records * 2 / readSkip);
	try
	{
		//首先进行分割
		//直接根据数据密度进行分块，大概每块2048个点左右
		double width   = (m_lasHeader.max_x - m_lasHeader.min_x);
		double height  = (m_lasHeader.max_y - m_lasHeader.min_y);
		double scaleWH = width / height;
		//每个块的大小
		double widthPer		= width  / (sqrt(totalLasNumber/2048 / scaleWH)) + 0.5;;
		double heightPer    = height / (sqrt(totalLasNumber / 2048 * scaleWH)) + 0.5;;
		int widthNum  =ceil( width / widthPer);
		int heightNum = ceil( height / heightPer);

		//反正先统计信息
		//分配内存
		m_lasDataset.LASRect_AllocateMemory(widthNum*heightNum, inMemory);
		//构建R树结构
		for (int i = 0; i < widthNum; ++i)
		{
			for (int j = 0; j < heightNum; ++j)
			{
				m_lasDataset.m_lasRectangles[j*widthNum + i].m_Rectangle.setRect(i*widthPer + m_lasHeader.min_x, j*heightPer + m_lasHeader.min_y,
					(i + 1)*widthPer + m_lasHeader.min_x, (j + 1)*heightPer + m_lasHeader.min_y);
			}
		}

		//感觉遍历了两遍 效率有点低，但是处理起来效率比较高
		if (m_Progress != NULL)
			m_Progress->SetMessage("read las file...");
		m_lasDataset.LASRect_BuildTree();
		pointsRect = new int[widthNum*heightNum];
		memset(pointsRect, 0, sizeof(int)*widthNum*heightNum);
		int alread_read = 0;
		int read_once_max = 2048;
		int read_once = m_lasHeader.number_of_point_records;

		long bytelen = read_once_max * m_lasHeader.point_data_record_length;
		readOnce = new unsigned char[bytelen];

		while (alread_read<m_lasHeader.number_of_point_records)
		{
			if (read_once>read_once_max)
				read_once = read_once_max;
			int readLen= read_once * m_lasHeader.point_data_record_length;
			fread(readOnce, readLen, 1, m_lasFile);
			//先读取，然后进行处理
			for (size_t i = 0; i < read_once; i += readSkip)
			{
				if (m_Progress != NULL)
					m_Progress->StepIt();
				LAS_XYZ vex;
				const unsigned char* data = readOnce + i*m_lasHeader.point_data_record_length;
				int size = sizeof(int);
				int x, y, z;
				memcpy(&x, data, size); memcpy(&y, data + size, size); memcpy(&z, data + size * 2, size);
				vex.x = x * m_lasHeader.x_scale_factor + m_lasHeader.x_offset;
				vex.y = y * m_lasHeader.y_scale_factor + m_lasHeader.y_offset;
				vex.z = z * m_lasHeader.z_scale_factor + m_lasHeader.z_offset;
				//fseek(m_lasFile, m_lasHeader.point_data_record_length - 12, SEEK_CUR);
				int widtnIdx = int((vex.x - m_lasHeader.min_x) / widthPer);
				int heighIdx = int((vex.y - m_lasHeader.min_y) / heightPer);
				pointsRect[heighIdx*widthNum+ widtnIdx]++;
			}
			alread_read += read_once;
			read_once_max = min(read_once_max, (m_lasHeader.number_of_point_records - alread_read));
		}

		//回到数据起点
		fseek(m_lasFile, m_lasHeader.offset_to_point_data, SEEK_SET);
		//第二遍遍历获取数据
		for (int j = 0; j < widthNum*heightNum; ++j)
		{
			m_lasDataset.m_lasRectangles[j].LASRect_AllocateMemory(pointsRect[j], inMemory, m_lasDataset.m_lasRectangles[j].m_Rectangle);
		}
		memset(pointsRect, 0, sizeof(int)*widthNum*heightNum);
		if (inMemory)
		{
			alread_read = 0;
			read_once_max = 2048;
			read_once = m_lasHeader.number_of_point_records;
			while (alread_read<m_lasHeader.number_of_point_records)
			{
				if (read_once>read_once_max)
					read_once = read_once_max;
				int readLen = read_once * m_lasHeader.point_data_record_length;
				fread(readOnce, readLen, 1, m_lasFile);
				//先读取，然后进行处理
				int totallasPnts = 0;
				for (size_t i = 0; i < read_once; i += readSkip)
				{
					if (m_Progress != NULL)
						m_Progress->StepIt();
					int num = sizeof(LASPoint);
					num = sizeof(LAS_XYZ);
					LASPoint vex;
					const unsigned char* data = readOnce + i*m_lasHeader.point_data_record_length;
					int size = sizeof(int);
					int x, y, z;
					memcpy(&x, data, size); memcpy(&y, data + size, size); memcpy(&z, data + size * 2, size);
					vex.m_vec3d.x = x * m_lasHeader.x_scale_factor + m_lasHeader.x_offset;
					vex.m_vec3d.y = y * m_lasHeader.y_scale_factor + m_lasHeader.y_offset;
					vex.m_vec3d.z = z * m_lasHeader.z_scale_factor + m_lasHeader.z_offset;
					memcpy(&vex.m_intensity, data + 12, sizeof(unsigned short));
					memcpy(&vex.m_rnseByte, data + 14, sizeof(unsigned char));
					memcpy(&vex.m_classify, data + 15, sizeof(unsigned char));
					memcpy(&vex.m_scanAngle, data + 16, sizeof(char));
					memcpy(&vex.m_flightID, data + 17, sizeof(unsigned short));
					memcpy(&vex.m_userdata, data + 19, sizeof(char));

					int colorOffset = 20;
					if (m_lasHeader.HasGPSTime())
					{
						memcpy(&vex.m_gpsTime, data + 20, sizeof(double));
						colorOffset += 8;
					}
					if (m_lasHeader.HasLASColorExt6())
					{
						memcpy(&vex.m_colorExt, data + colorOffset, sizeof(LASColorExt));
					}

					int widtnIdx = int((vex.m_vec3d.x - m_lasHeader.min_x) / widthPer);
					int heighIdx = int((vex.m_vec3d.y - m_lasHeader.min_y) / heightPer);
					memcpy(m_lasDataset.m_lasRectangles[heighIdx*widthNum + widtnIdx].m_lasPoints + pointsRect[heighIdx*widthNum + widtnIdx],&vex,sizeof(LASPoint));
					pointsRect[heighIdx*widthNum + widtnIdx]++;
					m_lasDataset.m_lasRectangles[heighIdx*widthNum + widtnIdx].m_point_index[totallasPnts] = alread_read + i;
				}
				alread_read += read_once;
				read_once_max = min(read_once_max, (m_lasHeader.number_of_point_records - alread_read));
			}
		}
		else
		{
			alread_read = 0;
			read_once_max = 2048;
			read_once = m_lasHeader.number_of_point_records;
			while (alread_read<m_lasHeader.number_of_point_records)
			{
				if (read_once>read_once_max)
					read_once = read_once_max;
				long		bytelen = read_once * m_lasHeader.point_data_record_length;
				fread(readOnce, bytelen, 1, m_lasFile);
				//先读取，然后进行处理
				int totallasPnts = 0;
				for (size_t i = 0; i < read_once; i += readSkip)
				{
					if (m_Progress != NULL)
						m_Progress->StepIt();
					LASPoint vex;
					const unsigned char* data = readOnce + i*m_lasHeader.point_data_record_length;
					int size = sizeof(int);
					int x, y, z;
					memcpy(&x, data, size); memcpy(&y, data + size, size); memcpy(&z, data + size * 2, size);
					vex.m_vec3d.x = x * m_lasHeader.x_scale_factor + m_lasHeader.x_offset;
					vex.m_vec3d.y = y * m_lasHeader.y_scale_factor + m_lasHeader.y_offset;
					vex.m_vec3d.z = z * m_lasHeader.z_scale_factor + m_lasHeader.z_offset;

					int widtnIdx = ceil((vex.m_vec3d.x - m_lasHeader.min_x) / widthPer);
					int heighIdx = ceil((vex.m_vec3d.y - m_lasHeader.min_y) / heightPer);
					totallasPnts++;
					m_lasDataset.m_lasRectangles[heighIdx*widthNum + widtnIdx].m_point_index[totallasPnts] = alread_read + i;
				}
				alread_read += read_once;
				read_once_max = min(read_once_max, (m_lasHeader.number_of_point_records - alread_read));
			}
		}
	}
	catch (bad_alloc &e)
	{
		printf("%s\n", e.what());
		exit(-1);
	}
	if (pointsRect != NULL)
		delete[]pointsRect;
	if (readOnce != NULL)
		delete[]readOnce;
	pointsRect = NULL;
	readOnce = NULL;

	return 0;
}

//将LAS数据写入文件中，在写入过程中重新获取LAS数据范围
long LASLidarReader::LidarReader_Write(const char* pathLidar)
{
	if (m_lasDataset.m_lasRectangles == NULL)
	{
		printf("no las data\n");
		exit(-1);
	}
	//新建一个LASHeader
	LASHeader lasHeader(m_lasHeader);
	lasHeader.max_x = m_lasDataset.m_lasRectangles[0].m_lasPoints[0].m_vec3d.x;
	lasHeader.min_x = m_lasDataset.m_lasRectangles[0].m_lasPoints[0].m_vec3d.x;
	lasHeader.max_y = m_lasDataset.m_lasRectangles[0].m_lasPoints[0].m_vec3d.y;
	lasHeader.min_y = m_lasDataset.m_lasRectangles[0].m_lasPoints[0].m_vec3d.y;
	lasHeader.max_z = m_lasDataset.m_lasRectangles[0].m_lasPoints[0].m_vec3d.z;
	lasHeader.min_z = m_lasDataset.m_lasRectangles[0].m_lasPoints[0].m_vec3d.z;
	int totalPoints = 0;
	if (m_Progress != NULL)
		m_Progress->SetStepCount(m_lasDataset.m_numRectangles * 2);
	if (m_Progress != NULL)
		m_Progress->SetMessage("write las file...");
	for (size_t i = 0; i < m_lasDataset.m_numRectangles; i++)
	{
		if (m_Progress != NULL)
			m_Progress->StepIt();
		for (size_t j = 0; j < m_lasDataset.m_lasRectangles[i].m_lasPoints_numbers; ++j)
		{
			lasHeader.max_x = max(m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_vec3d.x, lasHeader.max_x);
			lasHeader.min_x = min(m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_vec3d.x, lasHeader.max_x);
			lasHeader.max_y = max(m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_vec3d.y, lasHeader.max_y);
			lasHeader.min_y = min(m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_vec3d.y, lasHeader.max_y);
			lasHeader.max_z = max(m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_vec3d.z, lasHeader.max_z);
			lasHeader.min_z = min(m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_vec3d.z, lasHeader.max_z);
		}
		totalPoints += m_lasDataset.m_lasRectangles[i].m_lasPoints_numbers;
	}
	FILE* fLasOut = NULL;
	if (fopen_s(&fLasOut, pathLidar, "wb") != 0)
		return -1;
	lasHeader.number_of_point_records = totalPoints;
	lasHeader.LASHeader_Write(fLasOut);
	//中间有问题 不知道怎么搞 变长字段还没有进行处理
	int sizeBuff = lasHeader.offset_to_point_data - sizeof(LASHeader);
	if (sizeBuff != 0)
	{
		char* buffer = new char[sizeBuff];
		memset(buffer, 0, sizeof(char) * sizeBuff);
		fwrite(buffer, 1, sizeBuff, fLasOut);
		delete[]buffer; buffer = NULL;
	}
	bool isGpsTime = lasHeader.HasGPSTime(), isColorEx = lasHeader.HasLASColorExt6();
	for (size_t i = 0; i < m_lasDataset.m_numRectangles; i++)
	{
		if (m_Progress != NULL)
			m_Progress->StepIt();
		for (size_t j = 0; j < m_lasDataset.m_lasRectangles[i].m_lasPoints_numbers; ++j)
		{
			int x = m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_vec3d.x / lasHeader.x_scale_factor - lasHeader.x_offset;
			int y = m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_vec3d.y / lasHeader.y_scale_factor - lasHeader.y_offset;
			int z = m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_vec3d.z / lasHeader.z_scale_factor - lasHeader.z_offset;
			fwrite(&x, sizeof(int), 1, fLasOut);
			fwrite(&y, sizeof(int), 1, fLasOut);
			fwrite(&z, sizeof(int), 1, fLasOut);
			fwrite(&m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_intensity, sizeof(unsigned short), 1, fLasOut);
			fwrite(&m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_rnseByte, sizeof(unsigned char), 1, fLasOut);
			fwrite(&m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_classify, sizeof(unsigned char), 1, fLasOut);
			fwrite(&m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_scanAngle, sizeof(unsigned char), 1, fLasOut);
			fwrite(&m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_userdata, sizeof(unsigned char), 1, fLasOut);
			fwrite(&m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_flightID, sizeof(unsigned short), 1, fLasOut);
			if(isGpsTime)
				fwrite(&m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_gpsTime, sizeof(double), 1, fLasOut);
			if(isColorEx)
				fwrite(&m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_colorExt, sizeof(LASColorExt), 1, fLasOut);
		}
	}
	fclose(fLasOut);
}