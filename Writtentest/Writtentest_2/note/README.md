# 嵌入式即時系統 Written test 2
###### tags: `aperiodic job scheduling`、`multiprocessor`

## :memo:  Aperiodic job Scheduling
### Define
- Aperiodic Jobs
    沒有deadline
- Sporadic Jobs
    有deadline
### Handling Aperiodic Jobs
- Background execution
- Interrupt-driven execution
- Polled execution
    - servers
    
## Background execution
- 沒有週期性(periodic jobs)工作要做時，我才來做非週期性工作(aperiodic jobs)
- 優點: 簡單執行
- 缺點: response time 很差

![](https://i.imgur.com/QGFKtlo.png)

## Interrupt-driven execution
- 非週期性工作為優先執行
- 優點: response time 短
- 缺點: 可能危害到週期性工作排程

### Improvement
- Slack stealing

![](https://i.imgur.com/pFxTMRI.png)

## Polled execution
- 預留一個server服務非週期性工作，讓其變成像是類週期性工作
- 若當前週期queue是空的，就馬上**放棄所有budget**，下一周期再執行
- 若非週期性工作在該周期執行完，就馬上放棄CPU
- 優點: 可用週期性排程測試
- 缺點: 當queue是空的，polling server就放棄budget進入下一個週期

![](https://i.imgur.com/oQsOoiE.png)

### Terminology
- Backlogged(存貨)
    Queue裡面有ready jobs
- Eligible(可服務)
    Queue裡面有ready jobs，且Budget不為0

### Bandwidth-preserving servers
- Deferrable servers(RM、EDF)
- Sporadic servers(RM only)
- Constant utilization server & total bandwidth server(EDF only)
- 以上這些server希望budget 留越晚越好
- 以上這些server希望budget 越早補越好

## Deferrable servers
- 若當前週期queue是空的，**budget還是滿的**，budget可以一直留著
- budget會固定週期性補到滿

![](https://i.imgur.com/WzJBbeI.png)

- 優點: 對於非週期性工作respones time最好
- 缺點: 可能會干擾下方週期性工作(double hit)

![](https://i.imgur.com/vl3rPWt.png)


## Deferrable servers 能不能排程
- RM
    - 用liu-and-layland utilization bound測試能不能排程
	
    ![](https://i.imgur.com/Zi6GuZZ.png)
	
    ```
    U(2)<->0.6/3+0.8/4
    U(3)<->0.6/3+0.8/4+(0.5+0.8)/5
    U(4)<->0.6/3+0.8/4+0.5/5+(1.4+0.8)/7
    ```
- EDF

![](https://i.imgur.com/36pz3YQ.png)



## Simeple Sporadic servers
- CPU執行High priority task時budget不動
- CPU執行Low priority task時budget會依照速率1減少
- CPU idle時budget會依照速率1減少
- 並在idle結束就補滿budget

![](https://i.imgur.com/aMXmA8z.png)

## Sporadic servers
- server可以裂開(1.5,5) -> (1,5)、(0.5,5)，所以budget留著，當作還沒開始執行

![](https://i.imgur.com/cdnDtG6.png)

## Sporadic servers 能不能排程
- 加起來和U(4)做比較

## Priority-Exchange servers
- for EDF
- 製作複雜

![](https://i.imgur.com/xnWKIcG.png)


## Constant utilization server 
- for EDF
- 定義名詞:
    - arrival time: a 
    - execution time: c 
    - deadline: d
    - density: c/(d-a) 
- 如果任意時間density總和都<=1，可以排程
- 會給U=0.25，如果task執行時間=1，那task deadline=**3**(抵達時間)+**4**(1/0.25)=**7**
- 所以這server就是把deadline設好即可
- **server給的deadline < 原本task的deadline**

![](https://i.imgur.com/ihVH5U3.png)

## Total bandwidth server
- 補充時間提早，但dealine不變

![](https://i.imgur.com/pZw1l9L.png)



---

## :memo:  Multiprocessor Real-Time Scheduling

### Outline

- Mutiprocessor Real-Time Scheduling
- Global Scheduling
- Partitioned Scheduling
- Semi-partitioned Scheduling


### Mutiprocessor Models
- Identical processors
- Uniform processors
- Unrelated processors


### Scheduling Models
- Global Scheduling
    - 一個job可以被dispatched到任何CPU上處理
    - 一個job可以被migrate到任何CPU上處理
    - 只有一個global ready queue
- Partitioned Scheduling
    - tasks已經在CPU之間靜態的被分配好了
    - 不予許Task migration
    - 每一個CPU有自己的ready queue
- Semi-partitioned Scheduling
    - 有限度的予許Task migration

![](https://i.imgur.com/0uBQ7lW.png)



## Global Scheduling
- Global-EDF
當一個工作結束或一個新工作到達，the M processor executes M ready jobs having the M shortest deadlines
- Global-RM
當一個工作結束或一個新工作到達，the M processor executes M ready jobs having the M shortest periods
- Advantages
    - 較好的CPU使用率
- Disadvantages
    - 不好分析
    - 異常現象特別多，例如:週期放寬居然會讓系統變不能排程
- Anomaly 1: Relaxing Task Period

![](https://i.imgur.com/YhSqCSw.png)

- Anomaly 2: Dhall's Effect

![](https://i.imgur.com/2GhpFyO.png)

- Schedulability Test
    - 滿足以下公式就可以使用Global EDF排程，
    - M: M個CPU
    - Ck/Tk: 是Tasks中最大使用率
	
![](https://i.imgur.com/44tB7TT.png)

- Weakness of Global Scheduling
    - 大量的Migration
        - Cache re-population，大量cache miss發生 
        - Pipeline stall

## Partitioned Scheduling
- Advantages
    - 大部分在單CPU上使用的排程演算法皆可使用
- Disadvantages
    - worst case下使用率可能會很差
- Bin Packing Problem 是NP-Hard問題，可以被傳換成Partitioned Scheduling Problem
- Example

![](https://i.imgur.com/nHPYmBe.png)
- Partitioning Algorithms
    - First-Fit
        - 一直從index最小的開始排任務進去
    - Best-Fit
        - 找洞最小的排進去
    - Worst-Fit
        - 找洞最大的排進去
- Schedulability Test
    - 適用EDF-FF(fast fit)
    - m: m個CPU
    - α: 是Tasks中最大使用率
    - β: 是α的倒數取floor function
	
![](https://i.imgur.com/wd7PTkJ.png)


## Global vs Partitioned Scheduling
- Case 1
    - 假設只有2個CPU
    - 如果使用Global可以排程(參考上方)，但使用Partitioned Fast Fit，不可以排程
    
    | Task | P   | C  | U   |
    | ---- | --- | -- | --- |
    | T1   | 3   | 2  | 0.66|
    | T2   | 4   | 2  | 0.5 |
    | T3   | 12  | 8  | 0.66|

- Case 2
    - 假設只有2個CPU
    - 如果使用Global不可以排程(參考上方)，但使用Partitioned Fast Fit，可以排程
    
    | Task | P   | C  | U   |
    | ---- | --- | -- | --- |
    | T1   | 4   | 2  | 0.5 |
    | T2   | 4   | 2  | 0.5 |
    | T3   | 12  | 8  | 0.66|


## Semi-partitioned Scheduling

- 只有在保存下來的time window裡才能執行split task
- 剩餘時間，各別使用partitioned scheduling EDF排程

![](https://i.imgur.com/z0jhnlq.png)
