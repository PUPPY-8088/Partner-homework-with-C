#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//https://rtoax.blog.csdn.net/article/details/80309077
typedef const int cint;
typedef const char cchar;

/*一个手写数字的结构体*/
typedef struct
{
    int pixel[1024];
    int label;
}Digit;

/*一个有label的距离结构体*/
typedef struct
{
    float distance;
    int label;
}Distance;

/*文件路径+名称*/
cchar *trainingFile = "./my-digit-training.txt";
cchar *testingFile  = "./my-digit-testing.txt";
cchar *predictFile  = "./my-digit-predict.txt";

/*每个数据集的数字个数*/
cint   ntrain = 943;
cint    ntest = 196;
cint npredict = 9;

float calDistance(Digit digit1, Digit digit2)
/*求距离*/
{
    int i, squareSum=0.0;
    for(i=0;i<1024;i++)
    {
        squareSum += pow(digit1.pixel[i]-digit2.pixel[i], 2.0);
    }
    return sqrtf(squareSum);
}

/*读取digit*/
int loadDigit(Digit *digit, FILE *fp, int *labels)
{
    int index=0;
    for(index = 0; index<1024; index++)
    {
        if(!fscanf(fp, "%d", &digit->pixel[index]) )
        {
            printf("FILE already read finish.\n");
            return -1;
        }
    }

    fscanf(fp, "%d", &digit->label);
    *labels = digit->label;

    return 1;
}

/*显示一个Digit 结构体*/
void showDigit(Digit digit)
{
    int i, j, id;
    for(i=0;i<32;i++)
    {
        for(j=0;j<32;j++)
        {
            printf("%d", digit.pixel[i*32+j]);
        }
        printf("\n");
    }
    printf(" %d \n", digit.label);
}

/*交换字符串两项*/
void exchange(Distance *in, int index1, int index2)
{
    Distance tmp = (Distance)in[index1];
    in[index1] = in[index2];
    in[index2] = tmp;
}

/*选择排序*/
void selectSort(Distance *in, int length)
{
    int i, j, min;
    int N = length;
    for(i=0;i<N-1;i++)
    {
        min = i;
        for(j=i+1;j<N;j++)
        {
            if(in[j].distance<in[min].distance) min = j;
        }
        exchange(in,i,min);
    }
}

/*利用训练数据预测一个数据digit*/
int prediction(int K, Digit in, Digit *train, int nt)
{
    int i, it;
    Distance distance[nt];
    /*求取输入digit与训练数据的距离*/
    for(it=0; it<nt; it++)
    {
        distance[it].distance = calDistance(in, train[it]);
        distance[it].label = train[it].label;
    }
    /*给计算的距离排序(选择排序)*/
    int predict = 0;
    selectSort(distance, nt);
    for(i=0; i<K; i++)
    {
        predict += distance[i].label;
    }
    return (int)(predict/K);
}

/*用测试数据集进行测试*/
void knn_classifiy(int K)
{
    printf(".knn_classifiy.\n");

    int i;
    FILE *fp;

    /*读入训练数据*/
    int trainLabels[ntrain];
    int trainCount[10] = {0};
    Digit *Dtrain = (Digit*)malloc(ntrain*sizeof(Digit));
    fp = fopen(trainingFile,"r");
    printf("..load training digits.\n");
    for(i=0;i<ntrain;i++)
    {
        loadDigit(&Dtrain[i], fp, &trainLabels[i]);
        trainCount[Dtrain[i].label] ++;
    }
    fclose(fp);
    printf("..Done.\n");

    /*读入测试数据*/
    int testLabels[ntest];
    int testCount[10] = {0};
    Digit *Dtest = (Digit*)malloc(ntest*sizeof(Digit));
    fp = fopen(testingFile,"r");
    printf("..load testing digits.\n");
    for(i=0;i<ntest;i++)
    {
        loadDigit(&Dtest[i], fp, &testLabels[i]);
        testCount[Dtest[i].label] ++;
    }
    fclose(fp);
    printf("..Done.\n");

    /*求测试数据与训练数据之间的距离*/
    printf("..Cal Distance begin.\n");
    Distance Distance2Train[ntrain];
    int CorrectCount[10] = {0};
    int itrain, itest, predict;
    for(itest=0; itest<ntest; itest++)
    {
        predict = prediction(K, Dtest[itest], Dtrain, ntrain);
        //printf("%d-%d\n",predict, Dtest[itest].label);

        /*给预测准确的进行计数*/
        if(predict == Dtest[itest].label)
        {
            CorrectCount[predict] ++;
        }
    }

    /*输出测试数据的准确率*/
    printf("    Correct radio:   \n\n");
    for(i=0;i<10;i++)
    {
        printf("%d:  (  %2d / %2d ) =  %.2f%%\n",
                i,
                CorrectCount[i],
                testCount[i],
                (float)(CorrectCount[i]*1.0/testCount[i]*100));
    }
}
/*预测数据*/
void knn_predict(int K)
{
    int i;
    FILE *fp;

    /*读入训练数据*/
    int trainLabels[ntrain];
    int trainCount[10] = {0};
    Digit *Dtrain = (Digit*)malloc(ntrain*sizeof(Digit));
    fp = fopen(trainingFile,"r");
    printf("..load training digits.\n");

    for(i=0;i<ntrain;i++)
    {
        loadDigit(&Dtrain[i], fp, &trainLabels[i]);
        trainCount[Dtrain[i].label] ++;
    }

    fclose(fp);
    printf("..Done.\n");

    /*读入需要预测的数据*/
    int predictLabels[npredict];
    int predictCount[10] = {0};
    Digit *Dpredict = (Digit*)malloc(npredict*sizeof(Digit));
    fp = fopen(predictFile,"r");
    printf("..load predict digits.\n");

    for(i=0;i<npredict;i++)
    {
        loadDigit(&Dpredict[i], fp, &predictLabels[i]);
        predictCount[Dpredict[i].label] ++;
    }

    fclose(fp);
    printf("..Done.\n");

    /*求输入数据与训练数据之间的距离*/
    printf("..Cal Distance begin.\n");
    Distance Distance2Train[ntrain];
    int itrain, ipredict, predict;
    for(ipredict=0; ipredict<npredict; ipredict++)
    {
        predict = prediction(K, Dpredict[ipredict], Dtrain, ntrain);
        printf("%d\n",predict);
    }
}


int main(int argc, char** argv)
{
    int K = 1;
    /*对已知数据进行测试，统计预测的正确率*/
    knn_classifiy(K);
    /*对位置数据进行预测*/
    knn_predict(K);

    return 1;
}