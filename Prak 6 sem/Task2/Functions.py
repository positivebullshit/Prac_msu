from math import pi

ALPHABET=['A','B','C','F','!','^','&','$','|','[',']']
PARAMETERS=['l','w']


d1=100*pi/180 # divergence angle 1
d2=140*pi/180 # divergence angle 2
a=-18.95*pi/180 # branching angle
lr=1.15 # elongation rate
vr=1.45 #width increase rate

AXIOME='!(1.9)F(40)|(pi/4)A()'
PRODUCTION=['A():True→!(1.5*vr)F(30)[&(a)!(vr)F(20)A()$()]|(d1)[&(a)!(vr)F(20)A()$()]|(d2)[&(a)!(vr)F(20)A()$()]',
             'F(l):True→F(l*lr)',
             '!(w):True→!(w*vr)']

AXIOMES=[AXIOME,AXIOME]
PRODUCTIONS=[PRODUCTION,PRODUCTION]


def split_production(production):
    predecessor=production.split(':')[0]
    condition=production.split(':')[1].split('→')[0]
    successor=production.split(':')[1].split('→')[1]
    return [predecessor, condition,successor]


def word_to_modules(word,alphabet):
    modules=[]
    module=''
    for character in word:
        if character in alphabet :
            modules.append(module)
            module=character
        else:
            if character not in ['[',']','{','}','°']:
                module=module + character
    modules.append(module) #append last module
    modules=modules[1:]
    return modules
        

def n_parameters(module):
    n=0
    for car in module:
        if car==',':
            n=n+1
    if '()' in module:
        return 0
    else:
        return n+1
    

def parameters(module):
    param=''
    if module not in ['[',']','{','}','°']:
        param=module.split('(')[1].split(')')[0].split(',')
    return param


def match(actual,production):
    [predecessor,condition,successor]=split_production(production)
    if predecessor[0]==actual[0]  and n_parameters(predecessor)==n_parameters(actual) :
        for i in range(n_parameters(actual)):
            exec(parameters(predecessor)[i] + '=' + parameters(actual)[i])
        if eval(condition)==True:
            return True
    return False


def apply(module,production,alphabet):
    [predecessor,condition,successor]=split_production(production)
    result=''
    for i in range(n_parameters(module)):
            exec(parameters(predecessor)[i] + '=' + parameters(module)[i])
    successor_modules = word_to_modules(successor,alphabet) #modules in the successor
    for module in successor_modules:
        if module in ['[',']','{','}','°']:
            actual=module
        else:
            actual=module[0]+'('
            for parameter in parameters(module):
                if parameter !='':
                    actual=actual+str(eval(parameter))+','
            if actual[-1]==',':
                actual=actual[:-1] #delete last comma
            actual=actual + ')'
        result=result+ actual
    return result


def next(word,productions,alphabet):
    modules=word_to_modules(word,alphabet)
    result=''
    for module in modules :
        i=0
        for production in productions:
            if match(module,production):
                result=result+apply(module,production,alphabet)
                i=i+1
        if i==0 or module in ['[',']','{','}','°']:
                result=result+module
    return result


def parametric_word(axiome,productions,alphabet,n):
    word = axiome
    for i in range(n):
        word=next(word,productions,alphabet)
    return word
