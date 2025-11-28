
function DoTranslate($langFilePath, $resourceFilePath, $outFilePath)
{
    Write-Host "translating $resourceFile using $langFileName..."
    
      #读取翻译内容
    $langContent = Get-Content $langFilePath 
    if($langContent -eq $null)
    {
        Write-Host "error! read nothing from $langFilePath"
        return
    }
    
    $dictionary=@{}
    #将翻译内容转为hash table
    ForEach($line in $langContent)
    {
        $pairs = $line.split("=");
        
        if($pairs.count -lt 2)
        {
            continue;
        }
        
        #删除字符串开头和结尾的引号
        $orgText       = $pairs[0].trim("`"")
        $translateText = $pairs[1].trim("`"")
        
        #删除字符串开头和结尾的空格
        $orgText      = $orgText.trim()
        $translateText = $translateText.trim()
        $dictionary[$orgText] = $translateText       
    }
    
    #按照原始字符串的长短对hash table排序, 按长度递减排列
    #这里"-Descending"开关指定递减排列
    $dictionary = $dictionary.GetEnumerator() | Sort-Object {$_.key.length} -Descending 
    
  
    #读取资源文件

    $resourceContent = Get-Content $resourceFilePath
    if($resourceContent -eq $null)
    {
        Write-Host "error! read nothing from $resourceFilePath"
        
        return
     }
     
   
   <#
    trap
    {
        
        Write-Host "trapped"
        Write-Host $($pair.key)
    }
    #>
   
    #依照翻译信息替换资源文件的文本
    ForEach($pair in $dictionary.GetEnumerator())
    {
        #$resourceContent -replace $pair.key, $pair.value
        $key=$pair.key       
        $key = Unescape $key                 
        $value=$pair.value
        $resourceContent = $resourceContent -replace $key,$value
        Write-Host "$($pair.key)=$($pair.value)"
    }
       
        
       #Write-Host $resourceContent       
       Out-file -filepath ($outFilePath) -inputobject $resourceContent -Encoding "UNICODE"
}

function Unescape($regExp)
{
    $specialChars = @("\", "(", ")","[", "]", "{","}","+", "*", "?","^","|")
    #将准备做正则表达式的字符串中的特殊字符转义。
    ForEach($char in $specialChars)
    {
        $regExp =  $regExp -replace "\$char","\$char"
    }
    return $regExp;
}

$translateFiles=@(`
@{"lang-file"=".\lang\IWB-译文-德语.lang"; "resource-file"=".\RES\DEU_RES\DEU_RES.rc"},`
@{"lang-file"=".\lang\IWB-译文-俄语.lang"; "resource-file"=".\RES\RUS_RES\RUS_RES.rc"},`
@{"lang-file"=".\lang\IWB-译文-法语.lang"; "resource-file"=".\RES\FRA_RES\FRA_RES.rc"},`
@{"lang-file"=".\lang\IWB-译文-葡语.lang"; "resource-file"=".\RES\PTG_RES\PTG_RES.rc"},`
@{"lang-file"=".\lang\IWB-译文-西语.lang"; "resource-file"=".\RES\ESP_RES\ESP_RES.rc"},`
@{"lang-file"=".\lang\IWB-译文-意大利语.lang"; "resource-file"=".\RES\ITA_RES\ITA_RES.rc"}`
)



$scriptFilePath = $MyInvocation.MyCommand.Path
$baseDir = Split-Path -Parent  $scriptFilePath

$outputDir=$baseDir + "\output"

#生成输出目录lang
New-Item -ItemType Directory -Force -Path $outputDir

for($i = 0; $i -lt $translateFiles.length; $i++)
{
    $langFilePath =  Join-Path -Path $baseDir -ChildPath $translateFiles[$i]["lang-file"]
    $resourceFilePath = Join-Path -Path $baseDir -ChildPath $translateFiles[$i]["resource-file"]
    
    $outFilePath = Split-Path -leaf  $resourceFilePath
    $outFilePath = $outputDir + "\" + $outFilePath
    
    DoTranslate  $langFilePath $resourceFilePath $outFilePath
}





