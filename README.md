# BIM-project



## 问题*

在IC-M Model上运行CELF算法时，我发现一个问题：CELF输出的种子集不稳定，基本每次运行输出的种子集都完全不一样。

进一步探究，发现对于size相同的集合，MC simulation的结果都基本等于size的大小。故CELF无法判断哪个集合更优。

在排除代码bug的原因后，发现在simulation时seed set对邻居节点的meet基本不成功。

对数据集进行探究，发现在BlogCatalog数据集中，每条边的meeting probability都非常小，平均值为0.0574587。

对比IC-M model的原论文的实验，他们用的DBLP数据集的meeting probability平均值为0.338765。

(Recall: meeting probability $m_{u,v}=\frac{c}{c+d_{out}(u)}$, where $c=5$) 意味着BlogCatalog数据集很稠密。

**个人意见**：可以把meeting probability改为在$\{0.2,0.3,...,0.7,0.8\}$中随机（参考原论文）或者换个数据集？

## 更新的东西

1. 写了estimate随机选点的邻居重合度的API。
   1. 思路：通过shuffle让每个点随机选k个邻居。
   2. $重合程度=\frac{被两个及以上源点选中的点}{所有被选中的点}$
   3. 简单跑了几遍,size=10,k=3时重合程度=0.04。有待进一步统计。
2. 大致比较了不同的随机数生成器的性能差异：minstd_rand（线性同余法）的速度比mt19937（马特赛特旋转）快约30%。最后选用了minstd_rand生成随机数。*原因：根据线性同余法的性质，在[0,1)范围内近似为平均分布，已经满足simulation的需要。*
3. 改善随机数并且使用-O3加速之后，MC simulation快的飞起，而且r=100和r=1000000的性能差距不大（意味着可以用r=10000正常实验了）
4. 重写了CELF。（原代码在遇到问题(*)时会出bug）

## 接下来要做的事

比较集合质量的差异。

## 实验大纲

一个framework,描述option1和option2

输入为：graph, diffusion model, simulation method, set S

输出为：S的邻居节点的子集

graph: 一个图

diffusion model: IC

simulation method: MC, sketch

在option2时需要实现的IM算法：degree, pgrank, CELF

## 一些algorithm的性能估计

MC_simulation : O(iteration_rounds * 尝试active的次数) 随机数生成器采用硬件式随机数。

enumeration : 复杂度与所有解空间的size相当

IC-M model，当m_{p,v} = 1且deadline = inf时，MC simulation结果与IC model一致。
