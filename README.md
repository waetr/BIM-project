# BIM-project

## 更新的东西

修改了一些可能的bug，稍微优化了一些性能有关的细节。

写了顶层experiment方法，目前可以设置好参数直接挂到服务器上跑，输出结果重定向到output/result.out中。

result0.out为上一次本地跑出的部分实验结果。



## 运行方法

```bash
mkdir build && cd build
cmake ..
make
./exp
```



## 实验大纲

一个framework,描述option1和option2

输入为：graph, diffusion model, simulation method, set S

输出为：S的邻居节点的子集

graph: 一个图

diffusion model: IC

simulation method: MC, sketch

在option2时需要实现的IM算法：degree, pgrank, CELF

## 更新日志

2022/7/17

1. 写了estimate随机选点的邻居重合度的API。$重合程度=\frac{被两个及以上源点选中的点}{所有被选中的点}$
2. 大致比较了不同的随机数生成器的性能差异：minstd_rand（线性同余法）的速度比mt19937（马特赛特旋转）快约30%。最后选用了minstd_rand生成随机数。*原因：根据线性同余法的性质，在[0,1)范围内近似为平均分布，已经满足simulation的需要。*
3. 改善随机数并且使用-O3加速之后，MC simulation快的飞起，而且r=100和r=1000000的性能差距不大（意味着可以用r=10000正常实验了）
4. 重写了CELF。（原代码在遇到问题(*)时会出bug）

2022/7/18


写了option 2的advance版本。

算法：

1. 把active paticipate set $A$的所有邻居拿出来，记为$\N_A$。*对于每个$v\in \N_A$, $A_v$表示有邻居为$v$的active paticipate的集合。*
2. 按照某种顺序（degree, pageRank, margimal influence(CELF)）对于每个$v\in \N_A$, 尝试将$v$加入到seed里。
3. 每次尝试，任意选择一个$u\in A_v$，标记$u$多选了一个邻居，并且把$v$加入seed里。如果$u$已经选满$k$个邻居了，就把$u$从$A_v$中删除，再从$A_v-\{u\}$中任选一个作为$u$再次尝试，直到$u$还没选满或者$A_v$为空为止。如果$A_v$为空，说明$v$没有对应的active paticipate满足条件了，于是放弃$v$。
4. 把所有$v\in \N_A$依次尝试加入，算法结束。

选用了某一组$size=3$，邻居重合率$=\frac{3}{7}$的paticipate set，$k=2$，在$IC$和$TC-IC$模型上分别测试，输出见test.log。

结论：option 1结果最准确。在邻居重合率高的情况下，option 2漏掉了很多节点，而advance method选的节点数和option 1一样多，且degree,CELF选择的seed和option 1一样。
