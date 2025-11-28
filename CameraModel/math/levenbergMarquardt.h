#pragma once

#include <Matrix.h>
#include <nr3.h>




//@参数:
//     pParams, 参数数组
//     nParamsCount,参数个苏
//     lpCtx, 上下文数据指针
//     
typedef void (* NLSQFUNC)(const double* pParams, int nParamsCount, void* lpCtx, std::vector<double>& vecFuncValues);



//有限差分计算雅克比行列式
//∂f(X)/∂(xi) = [f(X+δx) - f(X)]/δx
//
inline void FiniteDifference(
    const double* pVariables,//模型的当前参量数组
    int nVariableNumber,
    const double* pFuncs, //模型的当前函数值数组
    int nFuncNumber,
    NLSQFUNC func,//计算解析表达式的函数
    void* lpCtx,
    MatlabSimu::Matrix<double>& Jac, //雅克比行列式
    int* pNumFuncEval = NULL //保存解析表达式回调次数的内存地址
    )
{


    Jac.SetDimension(nFuncNumber, nVariableNumber);


    //计算每个变量的变化步长
    std::vector<double> chg;
    chg.resize(nVariableNumber);

    double eps = std::numeric_limits<double>::epsilon();
    double sqrt_eps = sqrt(eps);
    double diffMin = 1.0E-8;
    double diffMax = 1.0E-1;

    for(int i = 0; i < nVariableNumber; i++)
    {
        double current = pVariables[i];
        
        //Value of stepsize suggested in Trust Region Methods, Conn-Gould-Toint, section 8.4.3
        double change = sqrt_eps * SIGN(1, current) * max(abs(current), 1.0);
        
        //Make sure step size lies within DiffminChange and DiffMaxChange
        change = SIGN(1, change + eps) * min( max(abs(change) ,diffMin) ,diffMax);

        chg[i] = change;
    }


    std::vector<double> vecNewVariable;
    vecNewVariable.resize(nVariableNumber);

    for(int i = 0; i < nVariableNumber; i++)
    {
        vecNewVariable[i] = pVariables[i];
    }


    std::vector<double> vecFuncNewValue;
    
    for(int grad = 0; grad < nVariableNumber; grad ++)
    {
        double temp = vecNewVariable[grad];//备份参量原始值

        vecNewVariable[grad] += chg[grad];//给参量加上一个变化量

        //func函数调用
        func(&vecNewVariable[0], nVariableNumber,  lpCtx, vecFuncNewValue);

        for(int j = 0; j < nFuncNumber ; j++)
        {
            Jac[j][grad] = (vecFuncNewValue[j] - pFuncs[j]) / chg[grad];

        }

        vecNewVariable[grad] = temp;//恢复参量的原始值

    }

    if(pNumFuncEval)
    {
        *pNumFuncEval = nVariableNumber;
    }

}


//@功能:返回矢量模的平方。
//@参数:pVector, 矢量数组
//      nDimension, 矢量的维度
inline double norm_square(const double* pVector, int nDimension)
{
    double value = 0.0;
    for(int i = 0; i < nDimension; i++)
    {
        value += pVector[i] * pVector[i];
    }

    return value;
}


//@功能:返回矢量模的平方。
//@参数:v, 矢量数组
inline double norm_square(const std::vector<double>& v)
{
    return norm_square(&v[0], v.size());

}

//@功能:返回矢量的2模
//@参数:pVector, 矢量数组
//      nDimension, 矢量的维度
inline double norm(const double* pVector, int nDimension)
{
    double value = 0.0;
    for(int i = 0; i < nDimension; i++)
    {
        value += pVector[i] * pVector[i];
    }

    return sqrt(value);
}

//@功能:返回矢量的2模
//@参数:v, 输入矢量
//      
inline double norm(const std::vector<double>& v)
{
    return norm(&v[0], v.size());
}


//@功能:计算矢量的∝+模
//      max(abs(v))
inline double infinite_norm(const double * pVector, int nDimension)
{
    double result = (std::numeric_limits<double>::min)();
    
    for(int i = 0; i < nDimension; i++)
    {
        double value = fabs(pVector[i]);
        if( value > result)
        {
            result = value;
        }
    }

    return result;
}


//@功能:计算矢量的∝+模
//      max(abs(v))
inline double infinite_norm(const std::vector<double>& v)
{
      
      return infinite_norm(&v[0], v.size());
}

enum EOptimExitFlags
{
    E_OPTIM_EXIT_UNDEF                            = -1,
    E_OPTIM_EXIT_REACH_MAX_ITERATION              = 0,
    E_OPTIM_EXIT_CONVERGENT_TO_FUNC               = 1,
    E_OPTIM_EXIT_STEP_SMALLER_THAN_TOLERANCE      = 2,
    E_OPTIM_RESIDUE_CHANGE_SMALLER_THAN_TOLERANCE = 3,

};

//@功能:测试参数值是否收敛到迭代结束。
inline bool TestConvergence(
        double infNorm_gradF,  //the first-order term of Taylor series.
        double tolOpt       ,  //the tolerance for first-order term of Taylor series
        double relFactor,      //Initial first order optimality safeguard
        int iter,              //iteration number
        int maxIter,           //最大迭代次数
        double norm_step,      //the normal of model's variable increment vector
        double norm_Xout,      //the normal of model's variable vector
        double tolX,           //relateive tolerance for variable step and current variables
        double newSumSquare,   //the new S = min∑{FUN(X).^2}
        double oldSumSquare,   //the old S = min∑{FUN(X).^2}
        double tolFun,         //relative tolerance bwtween new sumSquare and old sumSquare
        double sqrtEps,
        EOptimExitFlags& eExitFlgs //output variable to store exit flag.
        )
{
    bool bDone = false;
    
    if(infNorm_gradF < tolOpt * relFactor) //inf_norm(gradF)/relFactor < tolOpt
    {//梯度逼近0
        eExitFlgs = E_OPTIM_EXIT_CONVERGENT_TO_FUNC;
        bDone = true;
    }
    else if(iter > 0 )
    {
        if(norm_step < tolX * (sqrtEps + norm_Xout))
        {//norm(step)/(sqrtEps + norm(Xout)) < tolX
            eExitFlgs = E_OPTIM_EXIT_STEP_SMALLER_THAN_TOLERANCE;
            bDone = true;
        }
        else if(fabs(newSumSquare - oldSumSquare) <= tolFun * oldSumSquare)
        {//abs(newF - oldF)/oldF <= tolFun
           eExitFlgs = E_OPTIM_RESIDUE_CHANGE_SMALLER_THAN_TOLERANCE;
           bDone = true;
        }
        else if(iter > maxIter)
        {
            eExitFlgs = E_OPTIM_EXIT_REACH_MAX_ITERATION;
            bDone = true;
        }

    }

    return bDone;
}

void DebugPrintHeader()
{
    printf("\n                                        First-Order                    Norm of \n");
    printf(" Iteration  Func-count    Residual       optimality      Lambda           step\n");
}

void DebugPrintStep(
    double iter,
    double numFunEvals,
    double sumSq,
    double infnorm_gradF,
    double lambda,
    double norm_step = 0.0)
{
    const char* formatStrFirstIter = " %5.0f       %5.0f   %13.6g    %12.3g %12.6g\n";
    const char* formatstr          = " %5.0f       %5.0f   %13.6g    %12.3g %12.6g   %12.6g\n";
    
    if(iter == 0 )
    {
        printf(
            formatStrFirstIter,
            iter,
            numFunEvals,
            sumSq,
            infnorm_gradF,
            lambda);
    }
    else
    {
        printf(
            formatstr,
            iter,
            numFunEvals,
            sumSq,
            infnorm_gradF,
            lambda,
            norm_step);
    }

}

//nlsq, nonlinear least square
//solving the problem of the form
//S = min∑{FUN(X).^2}, when FUN and X may be vectors or matrices
//
//Exit Condition:
//1. LSQNONLIN converged to a solution X.
//2. Change in X smaller than the specified tolerance
//3. Change in the residual smaller than the specified tolerance.
//4. Mangintude search direction smaller than the specified tolerance
//5. Maximum number of function evaluations or of iterations reached.
//
//S(X|n>) = S(X|(n-1)) + 2*J'r(X)*ΔX + 2*(ΔX)'*J'*J*(ΔX)
//
inline void levenbergMarquardt(
    double* pParams,              //输入/输出参数, 模型的变量的当前值的数组
    int nParamCount,              //参数个数
    NLSQFUNC func,                //计算解析表达式的函数
    void* pContext,               //解析函数的输入参数
    int maxIter = 1000,           //最大迭代次数
    double tolFun = 1e-4 ,        //Termination tolerance on the function value.
    double tolX = 1e-4            //Termination tolerance on the variable X
    )
{
    int iter = 0;
    int numFunEvals =  0;
    int evals = 0;
    bool bSuccessfulStep = true;
    double lambda = 1e-2;//LM 算法参数


    //雅克比行列式
    MatlabSimu::Matrix<double> Jac;
    MatlabSimu::Matrix<double> JT;

   
    //Jac'*Jac
    MatlabSimu::Matrix<double> JTJ;
    MatlabSimu::Matrix<double> JTJ_Aug;
    MatlabSimu::Matrix<double> JTJ_Aug_Inv;


    std::vector<double> vecFuncValues;
    std::vector<double> vecTrialFuncs;
    std::vector<double> vecStep ;//参数增量矢量。

    //尝试的参数
    std::vector<double> vecTrialParams;
    vecTrialParams.resize(nParamCount);


    double eps = std::numeric_limits<double>::epsilon();
    double sqrt_eps = sqrt(eps);
    EOptimExitFlags eOptimExitFlags;
    


    //计算FUN(X)
    func(pParams, nParamCount, pContext, vecFuncValues);
    numFunEvals = 1;
    

    //计算 Sum_square = ∑(FUN^2)
    double sum_Square = norm_square(&vecFuncValues[0], vecFuncValues.size());
    
    //第一次计算Jacobian矩阵
     FiniteDifference(pParams, nParamCount, &vecFuncValues[0], vecFuncValues.size(), func, pContext, Jac, &evals);
     numFunEvals += evals;


     //Jac.Print("J=\n");

     //gradF = J'*ΔX
     std::vector<double> gradF = Transpose(Jac) * vecFuncValues;

    double infNorm_gradF = infinite_norm(gradF);


     //Initial first order optimality with safeguard to prevent value of 0, used in stopping tests
     //relFactor = max(norm(gradF,Inf),sqrtEps);
     double relFactor =  max(infinite_norm(gradF), sqrt_eps);


     //tolOpt: tolerance used when checking the 1st-order optimality
     double tolOpt = 1e-4 * tolFun;


    double norm_step      = 0.0;
    double norm_Xout      = 0.0;
    double sumSquare      = norm_square(vecFuncValues);
    double trialSumSquare = 0.0;
    


    //判断初始值,是否已经最优?
    bool bDone =  TestConvergence(
        infNorm_gradF,
        tolOpt,
        relFactor,
        iter,
        maxIter,
        norm_step,
        norm_Xout,
        tolX,
        sumSquare,
        0.0,
        tolFun,
        sqrt_eps,
        eOptimExitFlags);
    

    JT = Transpose(Jac);
    JTJ = JT*Jac;

    DebugPrintHeader();
    DebugPrintStep(
        iter,
        numFunEvals,
        sum_Square,
        infNorm_gradF,
        lambda);



    //计算
    while(!bDone)
    {

        if(bSuccessfulStep)
        {
            JTJ_Aug = JTJ;
        }

        //生成(JTJ + λI)
        for(int r = 0; r < nParamCount; r++)
        {
            JTJ_Aug[r][r] = JTJ[r][r] + lambda;
        }

        //JTJ_Aug.Print("JTJ=\n");
        
        int nInfo = MatlabSimu::Inverse(JTJ_Aug, JTJ_Aug_Inv);

        if(nInfo != 0)
        {
            if(nInfo < 0)
               throw std::exception("invalid arguments");

             if(nInfo > 0)
                 throw std::exception("sigular matrix");
            break;
        }

        vecStep = JTJ_Aug_Inv * JT * vecFuncValues;//注意这里得到的step是要从当前参数中减去的矢量。
        
        for(int i=0; i < nParamCount; i++)
        {
            vecTrialParams[i] = pParams[i] - vecStep[i];
        }


        
        //用新的参数去计算FUN(X)
        func(&vecTrialParams[0], vecTrialParams.size(), pContext, vecTrialFuncs);
        numFunEvals += 1;

        trialSumSquare = norm_square(vecTrialFuncs);

        norm_step = norm(vecStep);

        if(trialSumSquare < sumSquare)
        {//成功

            if(bSuccessfulStep)
            {
                lambda *= 0.1;
            }

            for(int i=0; i < nParamCount; i++)
            {
                pParams[i]  = vecTrialParams[i];
            }

            vecFuncValues  = vecTrialFuncs;


            //重新计算雅克比行列式
            FiniteDifference(pParams, nParamCount, &vecFuncValues[0], vecFuncValues.size(), func, pContext, Jac, &evals);

             numFunEvals += evals;

            JT = Transpose(Jac);

            JTJ = JT * Jac;

            //gradF = J'*ΔX
            gradF = JT  * vecFuncValues;
            infNorm_gradF = infinite_norm(gradF);

            norm_Xout = norm(pParams, nParamCount);

             iter ++;
             
             DebugPrintStep(
                    iter,
                    numFunEvals,
                    trialSumSquare,
                    infNorm_gradF,
                    lambda,
                    norm_step);

        //判断是否满足推出条件
         bDone =  TestConvergence(
                infNorm_gradF,
                tolOpt,
                relFactor,
                iter,
                maxIter,
                norm_step,
                norm_Xout,
                tolX,
                trialSumSquare,
                sumSquare,
                tolFun,
                sqrt_eps,
                eOptimExitFlags);

            sumSquare = trialSumSquare;
           
            bSuccessfulStep = true;


         }
        else
        {
            lambda *= 10.0;
            bSuccessfulStep = false;
            double norm_TrialParams = norm(vecTrialParams);
            if(norm_step < tolX * (sqrt_eps + norm_TrialParams))
            {
                eOptimExitFlags = E_OPTIM_EXIT_STEP_SMALLER_THAN_TOLERANCE;
                bDone = true;
            }

        }

       
    }


}