# BIM-project

现在的目标：一个framework,描述option1和option2

输入为：graph, diffusion model, simulation method, set S

输出为：S的邻居节点的子集

graph: 一个图

diffusion model: IC

simulation method: MC, sketch

在option2时需要实现的IM算法：degree, pgrank, CELF

# 现在的进度

实现了1个diffusion model : IC/WC

实现了1个estimate method : MC simulation

实现了3个IM solver : degree, pagerank, CELF

封装了option 1，在main函数有实例实现。

封装了三个IM solver的option 2。

还没有比较两个option的解的质量。

存在的问题：MC太慢，枚举出的集合太多（即使保证所有集合都为极大的），在设置r=100时option 1勉强能跑。

# 一些algorithm的性能估计

MC_simulation : O(iteration_rounds * 尝试active的次数) 随机数生成器采用硬件式随机数。

enumeration : 复杂度与所有解空间的size相当
