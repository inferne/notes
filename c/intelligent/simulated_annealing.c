#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

/* 方程 */
float val(float x) {
    return x + 10 * sin(5 * x) + 7 * cos(4 * x);
}
/* 生成随机数 */
float nrand() {
    return rand() / (double)RAND_MAX;
}
/* 判断函数，判断是否接受新解 */
int judge(float dE, float t)
{
    int y;
    if (dE < 0) {
        y = 1;
    } else {
        float d = exp(-(dE / t));
        if (d > nrand()) {
            y = 1;
        } else {
            y = 0;
        }
    }

    return y;
}

/* 取值范围 */
int section_l = 0;
int section_h = 9;

/* 初始温度、停止温度与降温系数 */
float tmp = 1e5;
float tmp_min = 1e-3;
float alpha = 0.98;

/* 计数器 */
int counter = 0;

int main()
{
    srand((unsigned)time(NULL));
    
    /* 生成初始随机解 */
    float x_old = (section_h - section_l) * nrand() + section_l;
    float x_new = x_old;
    float s_old = val(x_old);
    float s_new = s_old;
    
    /* 退火 */
    while (tmp > tmp_min) {
        /* 随机扰动 */
        float delta = (nrand() - 0.5) * 3;
        x_new = x_old + delta;
        /* 扰动的值小于一半的区间范围时，可以用这种方法防止新值超出区间范围 */
        if (x_new < section_l || x_new > section_h) {
            x_new = x_new - 2 * delta;
        }

        s_new = val(x_new);

        /* 求差值，这里是找最大值而非最小值，所以不是s_new - s_old */
        float dE = s_old - s_new;

        /* 判断 */
        float j = judge(dE, tmp);
        if (j) {
            s_old = s_new;
            x_old = x_new;
            printf("c=%-4d, y=%.4f, x=%.4f\n", counter, s_old, x_old);
        }

        /* 只有当dE < 0的情况下才降温 */
        if (dE < 0) {
            tmp = tmp * alpha;
        } else {
            counter = counter + 1;
        }

        /* 当接受更差的解的概率太小时，若又长时间找不到更优的解，那么退出循环，结束算法 */
        if (counter > 10000) {
            break;
        }
    }

    printf("y=%.4f,x=%.4f\n", s_old, x_old);

    return 0;
}

/*
c=9775, y=24.7403, x=7.8315
c=9812, y=24.8076, x=7.8405
c=9881, y=24.8380, x=7.8469
c=9949, y=24.8552, x=7.8578
y=24.8552,x=7.8578
*/