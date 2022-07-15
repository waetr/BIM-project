# BIM-project

现在的目标：一个framework,描述option1和option2

输入为：graph, diffusion model, simulation method, set S

输出为：S的邻居节点的子集

graph: 一个图

diffusion model: IC

simulation method: MC, sketch

在option2时需要实现的IM算法：degree pgrank, CELF

# 现在的进度

在main函数简单的实现了option 1的大致流程。
存在的问题：MC太慢，枚举出的集合太多（即使保证所有集合都为极大的），导致跑的太慢，在电脑上跑不出什么像样的结果。
