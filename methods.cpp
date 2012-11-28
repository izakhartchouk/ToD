
#include "methods.h"

/*
<sup>2</sup> -- ������� ������
<sub>2</sub> -- ������ ������

*/

Methods::Methods()
{

}

QVector<Point> Methods::GetStandarts()
{
    return standart;
}

int Methods::DirectionCosines(int x1, int y1, int x2, int y2)
{
    return ((float)x1*(float)x2+(float)y1*(float)y2)/(sqrt(((float)x1*(float)x1+(float)y1*(float)y1)*((float)x2*(float)x2+(float)y2*(float)y2)));
}

int Methods::EuclideanDistance(int x1, int y1, int x2, int y2)
{
    return sqrt(((float)x1-(float)x2)*((float)x1-(float)x2)+((float)y1-(float)y2)*((float)y1-(float)y2));
}

int Methods::TanimotoDistance(int x1, int y1, int x2, int y2)
{
    return (x1*x2+y1*y2)/((x1*x1+y1*y1)+(x2*x2+y2*y2)-(x1*x2+y1*y2));
}

void Methods::StandartsCalculation(QVector<Point> sample)
{
    if(!standart.isEmpty())
        standart.clear();

    QVector<int> cl;                   //������

    //��������� ������� �� �������
    cl.append(sample.value(0).clas);
    for(int i=1;i<sample.size();i++)
    {
        if(!cl.contains(sample.value(i).clas))
            cl.append(sample.value(i).clas);
    }

    //������� ������� ��� ������� ������
    Point p;
    for(int i=0;i<cl.size();i++)
    {
        int sx=0,sy=0;
        int k=0;
        for(int j=0;j<sample.size();j++)
        {
            if(sample.value(j).clas==cl.value(i))
            {
                sx+=sample.value(j).x;
                sy+=sample.value(j).y;
                k++;
            }
        }
        p.x=sx/k;
        p.y=sy/k;
        p.clas=cl.value(i);
        standart.append(p);
    }
}

int Methods::Standarts(Point X,direction dir, int (*metric)(int, int, int, int))
{
    if(standart.isEmpty())
        return -1;

    QVector<int> dest;
    int value;
    int result;

    //������� ���������� ����� ������ �������� � ������������� ������
    dest.append(metric(X.x,X.y,standart.value(0).x,standart.value(0).y));
    value=dest.value(0);result=standart.value(0).clas;
    for(int i=1;i<standart.size();i++)
    {
        dest.append(metric(X.x,X.y,standart.value(i).x,standart.value(i).y));
        if( (value<dest.value(i) && dir==MAX) || (value>dest.value(i) && dir==MIN) )
        {
            value = dest.value(i);
            result = standart.value(i).clas;
        }else if(value==dest.value(i))
            return -2;
    }

    return standart.value(result).clas;
}

int Methods::K_Neighbors(Point X,int K,QVector<Point> sample,direction dir, int (*metric)(int, int, int, int))
{
    if(K>sample.size() || !sample.size() || !K)
        return -1;

    QVector<int> dest,analiz;
    QVector<int> cl;                   //������
    int result;

    //��������� ������� �� �������
    cl.append(sample.value(0).clas);
    for(int i=0;i<sample.size();i++)
    {
        if(!cl.contains(sample.value(i).clas))
            cl.append(sample.value(i).clas);
    }

    //�������� ������ �������
    QVector<int> ccount;
    for(int i=0;i<cl.size();i++)
        ccount.append(0);

    //������� ���������� ����� ������������� ������ � ������� �������
    for(int i=0;i<sample.size();i++)
    {
        dest.append(metric(X.x,X.y,sample.value(i).x,sample.value(i).y));
        analiz.append(dest.value(i));
    }
    //�������������� ���������� ��� �������� ������ K ���./����. ����������
    qSort(analiz);

    int j;
    if(dir==MAX)j=sample.size()-1;
    else j=0;

    //������� ���������� ������ �������
    while( (j-K>=0 && dir==MAX) || (j<=K-1 && dir==MIN) )
    {
        ccount[cl.indexOf(sample[dest.indexOf(analiz.value(j))].clas)]+=1;

        if(dir==MAX)j--;
        else j++;
    }

    int value=ccount[0],valuei=0;

    //� ������ ������ ������ ������?
    for(int i=1;i<ccount.size();i++)
    {
        if( value<ccount[i] )
        {
            value=ccount[i];
            valuei=i;
        }
        //else if(value==ccount[i])
        //    return -2;
    }

    return result=cl.value(valuei);
}

QVector<Point> Methods::findSplitLine(QVector<Point> srcpoints,int dy, int clas0, int clas1, int xMAX, int yMAX)
{
    QVector<Point> line,smartline,sample;
    int (*_metrix_)(int x1,int y1,int x2,int y2);
    Point P;
    int minX,minY,maxX,maxY;
    int sumUpX,sumDownX;
    sumUpX=sumDownX=0;

    _metrix_ = Methods::EuclideanDistance;


    //��������� �� ��������� ����� ��������� �������
    for(int i=0;i<srcpoints.size();i++)
    {
        if(srcpoints.value(i).classType==DEFINED && (srcpoints.value(i).clas==clas0 || srcpoints.value(i).clas==clas1))
            sample.append(srcpoints.value(i));
    }

    /*minX = maxX = sample.value(0).x;
    minY = maxY = sample.value(0).y;
    for(int i=1;i<sample.size();i++)
    {
        if(minX>sample.value(i).x)
            minX=sample.value(i).x;
        else if(maxX<sample.value(i).x)
            maxX=sample.value(i).x;

        if(minY>sample.value(i).y)
            minY=sample.value(i).y;
        else if(maxY<sample.value(i).y)
            maxY=sample.value(i).y;
    }*/

    StandartsCalculation(sample); //���������� ��������

    //������ �� ������ �������
    P.y=0;
    for(int x=0;x<xMAX;x++)
    {
        P.x=x;
        P.clas = -1;
        P.classType = UNDEFINED;
        if(Standarts(P,MIN,_metrix_)==-2)
            line.append(P);
    }

    //������ �� ���� ������� �������
    for(int y=1;y<yMAX-1;y+=dy)
    {
        P.y=y;
        P.clas = -1;
        P.classType = UNDEFINED;

        for(int x=0;x<xMAX;x+=1)
        {
            P.x=x;
            if(Standarts(P,MIN,_metrix_)==-2)
                line.append(P);
        }
    }

    //������ �� ������� �������
    P.y=yMAX;
    for(int x=0;x<xMAX;x++)
    {
        P.x=x;
        P.clas = -1;
        P.classType = UNDEFINED;
        if(Standarts(P,MIN,_metrix_)==-2)
            line.append(P);
    }

    smartline.append(line.value(0));
    smartline.append(line.last());

    /*for(int i=0;i<line.size()/2;i++)
        sumDownX+=line.value(i).x;
    for(int i=line.size()/2;i<line.size();i++)
        sumUpX+=line.value(i).x;

    P.x=sumDownX/(line.size()/2);
    P.y=minY;
    smartline.append(P);

    P.x=sumUpX/( (line.size()) - (line.size()/2) );
    P.y=maxY;
    smartline.append(P);*/

    /*if(smartline.value(0).y == smartline.value(1).y)
    {
        P1.x=xMAX;
        P2.x=0;
        P1.y=P2.y=smartline.value(0).y;
    }
    else if(smartline.value(0).x == smartline.value(1).x)
    {
        P1.x=P2.x=smartline.value(0).x;
        P1.y=0;
        P2.y=yMAX;
    }
    else if(smartline.value(0).x > smartline.value(1).x)
    {
        int sy = P2.y-P1.y;
        int sx = P1.x-P2.x;

        while((P1.y > 0) && (P1.x < xMAX))
        {
            P1.y-=sy;
            P1.x+=sx;
        }
        while((P2.y < yMAX) && (P2.x > 0))
        {
            P2.y+=sy;
            P2.x-=sx;
        }

        if(P1.y<0 && P1.x<xMAX)P1.y=0;
        if(P1.y>0 && P1.x>xMAX)P1.x=xMAX;
        if(P2.y>yMAX  && P2.x>0)P2.y=yMAX;
        if(P2.y<yMAX  && P2.x<0)P2.x=0;
    }
    else if(smartline.value(0).x < smartline.value(1).x)
    {
        int sy = P2.y-P1.y;
        int sx = P2.x-P1.x;

        while((P1.y>0) && (P1.x>0))
        {
            P1.y-=sy;
            P1.x-=sx;
        }
        while((P2.y<yMAX) && (P2.x<xMAX))
        {
            P2.y+=sy;
            P2.x+=sx;
        }

        if(P1.y<0 && P1.x>0)P1.y=0;
        if(P1.y>0 && P1.x<0)P1.x=0;
        if(P2.y>yMAX  && P2.x<xMAX)P2.y=yMAX;
        if(P2.y<yMAX  && P2.x>xMAX)P2.x=xMAX;
    }

    smartline.insert(0,P1);
    smartline.append(P2);*/

    return smartline;
}

ChartLine Methods::findSplitLines(QVector<Point> srcpoints, int dy,int xMAX,int yMAX)
{
    ChartLine lines;
    QVector<Point> defined;
    QVector<int> cl;

    for(int i=0;i<srcpoints.size();i++)
    {
        if(srcpoints.value(i).classType==DEFINED)
            defined.append(srcpoints.value(i));
    }
    //��������� �������
    cl.append(defined.value(0).clas);
    for(int i=1;i<defined.size();i++)
    {
        if(!cl.contains(defined.value(i).clas))
            cl.append(defined.value(i).clas);
    }

    //���������� �������� ����� ����������� ����� ����� ����� ��������
    for(int i=0;i<cl.size();i++)
    {
        for(int j=i+1;j<cl.size();j++)
            lines.addLine(findSplitLine(defined,dy,cl.value(i),cl.value(j),xMAX,yMAX),cl.value(i),cl.value(j));
    }

    return lines;
}

QVector<Point> Methods::calculateClass(QVector<Point> points, CalculateMethod method, CalculateMetrix metrix)
{
    QVector<Point> sample;
    QVector<Point> Xpoints;
    int (*_metrix_)(int x1,int y1,int x2,int y2);
    direction dir;

    //��������� �� ��������� ����� ��������� ������� � �����, ��� ����� ����������
    for(int i=0;i<points.size();i++)
    {
        if(points.value(i).classType==DEFINED)
            sample.append(points.value(i));
        else Xpoints.append(points.value(i));
    }

    //����������� ������� �������� ����������
    switch(metrix)
    {
        case CALCULATE_METRIX_EUCLIDEAN:
        {
            _metrix_ = Methods::EuclideanDistance;
            dir = MIN;
            break;
        }
        case CALCULATE_METRIX_DIRCOS:
        {
            _metrix_ = Methods::DirectionCosines;
            dir = MAX;
            break;
        }
        case CALCULATE_METRIX_TINIMOTO:
        {
            _metrix_ = Methods::TanimotoDistance;
            dir = MAX;
            break;
        }
    }

    //����� ������ ����������� �������������� ����������� ����� � ���� ����������� ��������������
    switch(method)
    {
        case CALCULATE_METHOD_STANTARD:
        {
            StandartsCalculation(sample);
            for(int i=0;i<Xpoints.size();i++)
            {
                Xpoints[i].clas=Standarts(Xpoints.value(i),dir,_metrix_);
                //! don't do it Xpoints[i].classType=DEFINED;
            }
            break;
        }
        case CALCULATE_METHOD_LINES:
        {
            StandartsCalculation(sample);
            for(int i=0;i<Xpoints.size();i++)
            {
                Xpoints[i].clas=Standarts(Xpoints.value(i),dir,_metrix_);
                //! don't do it Xpoints[i].classType=DEFINED;
            }
            break;
        }
        case CALCULATE_METHOD_K_NEIGBORDS:
        {
            int K = sample.size()/2;
            for(int i=0;(sample.size()+i)%2==0;i++)
                K = (sample.size()+i)/2;

            for(int i=0;i<Xpoints.size();i++)
            {
                Xpoints[i].clas=K_Neighbors(Xpoints.value(i),K,sample,dir,_metrix_);/////////////////////////////////////////////����� �� ����� �������?
                //! don't do it Xpoints[i].classType=DEFINED;
            }
            break;
        }
    }

    foreach(Point p, sample)
        Xpoints.append(p);

    return Xpoints;
}
