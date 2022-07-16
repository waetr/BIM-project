# BIM-project

## 接下来要做的事

把一些用vector的临时变量改成数组。因为vector实在太慢了。

## 实验大纲

一个framework,描述option1和option2

输入为：graph, diffusion model, simulation method, set S

输出为：S的邻居节点的子集

graph: 一个图

diffusion model: IC

simulation method: MC, sketch

在option2时需要实现的IM算法：degree, pgrank, CELF

## 现在的进度

实现了2个diffusion model : IC/WC, IC-M

实现了1个estimate method : MC simulation

实现了3个IM solver : degree, pagerank, CELF

封装了option 1，在main函数有实例实现。

封装了三个IM solver的option 2。

还没有比较两个option的解的质量。

存在的问题：MC simulation的IC-M版本实在是太慢了。

## 一些algorithm的性能估计

MC_simulation : O(iteration_rounds * 尝试active的次数) 随机数生成器采用硬件式随机数。

enumeration : 复杂度与所有解空间的size相当
