#include <stdio.h>
#include <string.h>   /* for all the new-fangled string functions */
#include <stdlib.h>     /* malloc, free, rand */

const int MAX_FORMULA_SIZE=50; /*maximum formula length*/
const int NUMBER_OF_INPUTS =10;/* number of formulas expected in input.txt*/
const int MAX_TAB_SIZE=500; /*maximum length of tableau queue, if needed*/

#define TRUE  1;
#define FALSE 0;
typedef int bool;

enum FormulaType
{
  NONE,
  PROPOSITION,
  NEGATION,
  BINARY
};

/* A tableau will contain the current formula, the left branch and the right branch, if there's only one branch the right branch will be null*/
typedef struct tableau {
  char* currentFormula;
  struct tableau *leftBranch;
  struct tableau *rightBranch;
} tableau;

bool formulaHasOpeningBracket(char *formula)
{
  if(formula[0] == '(')
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

bool formulaHasClosingBracket(char *formula)
{
  int lastCharPosition = strlen(formula) - 1;
  if(formula[lastCharPosition] == ')')
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

bool isBinaryConnective(char c)
{
  if (c == 'v' || c == '^' || c == '>')
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

int findBinaryConnectivePosition(char *formula)
{
  int bracketCount = 0;
  int i;
  for (i = 0; i < strlen(formula); i++)
  {
    if (formula[i] == '(')
    {
      bracketCount++;
    }
    if (formula[i] == ')')
    {
      bracketCount--;
    }
    if (isBinaryConnective(formula[i]) && bracketCount == 1)
    {
      return i;
    }
  }
  return -1;
}

char getBinaryConnective(char *formula)
{
  int connectivePosition = findBinaryConnectivePosition(formula);
  char binaryConnective = formula[connectivePosition];
  return binaryConnective;
}

char negatedBinaryConnective(char c)
{
  switch (c)
  {
    case('v'): return '^';
    case('^'): return 'v';
    case('>'): return '^';
    default: return ' '; //should never hit this case
  }
}

char *partone(char *formula)
{
  int formulaStart = 1;
  int connectivePosition = findBinaryConnectivePosition(formula); 
  int partOneLength = connectivePosition - 1;
  // add 1 for null character
  char *partOneArray = (char *)malloc(sizeof(char)*(partOneLength + 1));
  // char partOneArray[partOneLength + 1];
  int i;
  for (i = formulaStart; i < connectivePosition; i++)
  {
    partOneArray[i-formulaStart] = formula[i];
  }
  return partOneArray;
}
 
char *parttwo(char *formula) 
{
  int connectivePosition = findBinaryConnectivePosition(formula);
  int formulaStart = connectivePosition + 1;
  int partTwoLength = strlen(formula) - connectivePosition - 2;
  char *partTwoArray = (char *)malloc(sizeof(char)*(partTwoLength + 1));
  // add 1 for null character
  // char partTwoArray[partTwoLength + 1];
  int i;
  for (i = formulaStart; i < strlen(formula) - 1; i++)
  {
    partTwoArray[i-formulaStart] = formula[i];
  }
  return partTwoArray;
}
 
 
enum FormulaType parse(char *formula)
{
 
  if (strlen(formula) == 0)
  {
    return NONE;
  }
 
  // Proposition
  if (strlen(formula) == 1)
  {
    if (formula[0] == 'p' || formula[0] == 'q' || formula[0] == 'r')
    {
      return PROPOSITION;
    }
  }
 
  // Negation
  if (formula[0] == '-')
  {
    if (parse(formula + sizeof(char)) != NONE)
    {
      return NEGATION;
    }
    else
    {
      return NONE;
    }
  }
 
  // Binary
  if (formulaHasOpeningBracket(formula) && formulaHasClosingBracket(formula)) {
    int connectivePosition = findBinaryConnectivePosition(formula);
    if (connectivePosition != -1)
    {
      enum FormulaType part1type = parse(partone(formula));
      enum FormulaType part2type = parse(parttwo(formula));
      if (part1type != NONE && part2type != NONE)
      {
        return BINARY;
      }
    }
    
  }

  return NONE;
 
}

tableau *createTableauFromFormula(char *formula)
{
  tableau *t = (tableau *)malloc(sizeof(tableau));
  t->currentFormula = formula;
  t->leftBranch = NULL;
  t->rightBranch = NULL;
  return t;
}

tableau *copyTableau(tableau *t)
{
  if (t == NULL)
  {
    return NULL;
  }
  tableau *copy = (tableau *)malloc(sizeof(tableau));
  char *new = (char *)malloc(sizeof(char)*(strlen(t->currentFormula)+1));
  int i;
  for (i = 0; i < strlen(t->currentFormula); i++)
  {
    new[i] = t->currentFormula[i];
  }
  copy->currentFormula = new;
  copy->leftBranch = copyTableau(t->leftBranch);
  copy->rightBranch = copyTableau(t->rightBranch);
  return copy;
}

void addAlphaBranch(tableau *t, tableau *branch)
{
  if (t == NULL || branch == NULL) 
  {
    return;
  }

  if (t->leftBranch == NULL)
  {
    t->leftBranch = branch;
  }
  if (t->leftBranch != NULL && t->rightBranch != NULL)
  {
    addAlphaBranch(t->leftBranch, branch);
    addAlphaBranch(t->rightBranch, copyTableau(branch));
  }
}

void addBetaBranch(tableau *t, tableau *firstbranch, tableau *secondbranch)
{
  if (t == NULL || firstbranch == NULL || secondbranch == NULL)
  {
    return;
  }

  if (t->leftBranch == NULL)
  {
    t->leftBranch = firstbranch;
    t->rightBranch = secondbranch;
  }
  else if (t->leftBranch != NULL && t->rightBranch == NULL)
  {
    addBetaBranch(t->leftBranch, firstbranch, secondbranch);
  }
  else
  {
    //both branches not empty
    addBetaBranch(t->leftBranch, firstbranch, secondbranch);
    addBetaBranch(t->rightBranch, copyTableau(firstbranch), copyTableau(secondbranch));
  }
}

void alphaExpansion(tableau *t)
{
  char *leftPart = partone(t->currentFormula);
  char *rightPart = parttwo(t->currentFormula);
  tableau *firstAlpha = createTableauFromFormula(leftPart);
  tableau *secondAlpha = createTableauFromFormula(rightPart);

  firstAlpha->leftBranch = secondAlpha;
  addAlphaBranch(t, firstAlpha);
}

void betaExpansion(tableau *t)
{
  char *leftPart = partone(t->currentFormula);
  char *rightPart = parttwo(t->currentFormula);
  tableau *firstBeta = createTableauFromFormula(leftPart);
  tableau *secondBeta = createTableauFromFormula(rightPart);

  addBetaBranch(t, firstBeta, secondBeta);
}

void freeTableau(tableau *t) {
  if (t != NULL) {
    freeTableau(t->leftBranch);
    freeTableau(t->rightBranch);
    free(t->currentFormula);
    free(t);
  }

}

bool isNegatedProposition(char *formula)
{
  return (parse(formula) == NEGATION && parse(formula + 1) == PROPOSITION);
}

bool isPropositionOrNegatedProposition(char *formula)
{
  return (parse(formula) == PROPOSITION || isNegatedProposition(formula));
}

char *rewriteImplies(char *formula)
{
  char firstBracket = '(';
  char negation = '-';
  char *leftPart = partone(formula);
  char binaryConnective = 'v';
  char *rightPart = parttwo(formula);
  char secondBracket = ')';

  int rewrittenFormulaLength = strlen(formula) + 1;
  char *rewrittenFormula = (char *)malloc(sizeof(char)*(rewrittenFormulaLength+1));

  int currentIndex = 0;
  rewrittenFormula[0] = firstBracket;
  currentIndex++;
  rewrittenFormula[1] = negation;
  currentIndex++;
  int i;
  for (i = 0; i < strlen(leftPart); i++)
  {
    rewrittenFormula[currentIndex] = leftPart[i];
    currentIndex++;
  }
  rewrittenFormula[currentIndex] = binaryConnective;
  currentIndex++;
  for (i = 0; i < strlen(rightPart); i++)
  {
    rewrittenFormula[currentIndex] = rightPart[i];
    currentIndex++;
  }
  rewrittenFormula[currentIndex] = secondBracket;
  currentIndex++;
  rewrittenFormula[currentIndex] = '\0';
  return rewrittenFormula;
}

char *rewriteNegation(char *formula)
{
  // assume the rest of the formula is a negation or a binary
  if (parse(formula + 1) == NEGATION)
  {
    // removing the first two characters of the formula because negations cancel out
    int formulaStart = 2;
    int formulaLength = strlen(formula);
    int rewrittenFormulaLength = formulaLength - 2;
    // plus 1 for null character
    char *rewrittenFormula = (char *)malloc(sizeof(char)*(rewrittenFormulaLength+1));
    int i;
    for (i = formulaStart; i < formulaLength; i++)
    {
      rewrittenFormula[i-formulaStart] = formula[i];
    }
    return rewrittenFormula;
  }
  else
  {
    // remaining formula is a binary
    // add a not before left part of the formula and before the right part of the formula and change the binary connective and remove the initial negation.
    // if the connective is an implication, add two nots before left part of the formula, and one not before the right part of the formula.
    char *remainingFormula = formula + 1;

    char firstBracket = '(';
    char *firstNegation;
    char *leftPart = partone(remainingFormula);
    char binaryConnective = negatedBinaryConnective(getBinaryConnective(remainingFormula));
    char secondNegation = '-';
    char *rightPart = parttwo(remainingFormula);
    char secondBracket = ')';

    // consider putting this in a function
    if (getBinaryConnective(remainingFormula) == '>')
    {
      firstNegation = (char *)malloc(sizeof(char)*(2+1));
      firstNegation[0] = '-';
      firstNegation[1] = '-';
    }
    else
    {
      firstNegation = (char *)malloc(sizeof(char)*(1+1));
      firstNegation[0] = '-';
    }

    // put together into one array
    int rewrittenFormulaLength = 1 + strlen(firstNegation) + strlen(leftPart) + 1 + 1 + strlen(rightPart) + 1;
    char *rewrittenFormula = (char *)malloc(sizeof(char)*(rewrittenFormulaLength + 1));

    int currentIndex = 0;
    rewrittenFormula[0] = firstBracket;
    currentIndex++;
    int i;
    for(i = 0; i < strlen(firstNegation); i++)
    {
      rewrittenFormula[currentIndex] = firstNegation[i];
      currentIndex++;
    }
    for(i = 0; i < strlen(leftPart); i++)
    {
      rewrittenFormula[currentIndex] = leftPart[i];
      currentIndex++;
    }
    rewrittenFormula[currentIndex] = binaryConnective;
    currentIndex++;
    rewrittenFormula[currentIndex] = secondNegation;
    currentIndex++;
    for (i = 0; i < strlen(rightPart); i++)
    {
      rewrittenFormula[currentIndex] = rightPart[i];
      currentIndex++;
    }
    rewrittenFormula[currentIndex] = secondBracket;
    currentIndex++;
    rewrittenFormula[currentIndex] = '\0';

    return rewrittenFormula;
  }
}

void complete(tableau *t){
  if (t == NULL)
  {
    return;
  }
  if (t->currentFormula == NULL)
  {
    return;
  }
  switch(parse(t->currentFormula))
  {
    case(NONE): return;
    case(PROPOSITION):
      complete(t->leftBranch);
      complete(t->rightBranch);
      break;
    case(NEGATION): 
      if (isNegatedProposition(t->currentFormula))
      {
        complete(t->leftBranch);
        complete(t->rightBranch);
        break;
      }
      else
      {
        char *rewritten = rewriteNegation(t->currentFormula);
        t->currentFormula = rewritten;
        complete(t);
      }
      break;
    case(BINARY): 
      if (getBinaryConnective(t->currentFormula) == '^')
      {
        alphaExpansion(t);
        complete(t->leftBranch);
        complete(t->rightBranch);
      }
      else if (getBinaryConnective(t->currentFormula) == 'v')
      {
        betaExpansion(t);
        complete(t->leftBranch);
        complete(t->rightBranch);
      }
      else
      {
        // binary connective is implies
        char *rewritten = rewriteImplies(t->currentFormula);
        t->currentFormula = rewritten;
        complete(t);
      }
      break;
  }
  return;
}

void printTableau(tableau *t)
{
  printf("Tableau t current formula: %s\n", t->currentFormula);
  if(t->leftBranch != NULL)
  {
    printTableau(t->leftBranch);
  }
  else
  {
    printf("No left branch\n");
  }
  if(t->rightBranch != NULL)
  {
    printTableau(t->rightBranch);
  }
  else
  {
    printf("No right branch\n");
  }
  return;
}

char *negatedProposition(char *proposition)
{
  if (isNegatedProposition(proposition))
  {
    char *negated = (char *)malloc(sizeof(char)*(1+1));
    negated[0] = proposition[1];
    return negated;
  }
  else
  {
    char *negated = (char *)malloc(sizeof(char)*(2+1));
    negated[0] = '-';
    negated[1] = proposition[0];
    return negated;
  }
}

bool stringsAreEqual(char *a, char *b)
{
  if (strlen(a) != strlen(b))
  {
    return FALSE;
  }
  int i;
  for (i = 0; i < strlen(a); i++)
  {
    if(a[i] != b[i])
    {
      return FALSE;
    }
  }
  return TRUE;
}

bool checkForContradiction(tableau *t, char *currentProposition)
{
  if (t == NULL) 
  {
    return FALSE;
  }
  if (stringsAreEqual(t->currentFormula, negatedProposition(currentProposition)))
  {
    return TRUE;
  }
  else
  {
    if (checkForContradiction(t->leftBranch, currentProposition)) 
    {
      return TRUE;
    }
    return (checkForContradiction(t->rightBranch, currentProposition));
  }
}

bool closed(tableau *t)
{
  if (t == NULL)
  {
    return FALSE;
  }
  if (t->leftBranch == NULL && t->rightBranch == NULL)
  {
    return FALSE;
  }
  if (t->leftBranch != NULL && t->rightBranch == NULL)
  {
    if (isPropositionOrNegatedProposition(t->currentFormula))
    {
      return checkForContradiction(t->leftBranch, t->currentFormula);
    }
    else
    {
      return closed(t->leftBranch);
    }
  }
  else
  {
    //left and right branch are not empty
    if (isPropositionOrNegatedProposition(t->currentFormula))
    {
      return (checkForContradiction(t->leftBranch, t->currentFormula) && checkForContradiction(t->rightBranch, t->currentFormula));
    }
    else
    {
      return (closed(t->leftBranch) && closed(t->rightBranch));
    }
  }
}


int main(){

 /*input 10 strings from "input.txt" */
    FILE *fp, *fpout;
    tableau *t;
 
  /* reads from input.txt, writes to output.txt*/
    if ((  fp=fopen("input.txt","r"))==NULL){printf("Error opening file");exit(1);}
    if ((  fpout=fopen("output.txt","w"))==NULL){printf("Error opening file");exit(1);}

    int j;
    
    for(j=0;j<NUMBER_OF_INPUTS;j++)
    {

        char *formula = malloc(MAX_FORMULA_SIZE);
        fscanf(fp, "%s",formula);/*read formula*/
        switch (parse(formula))
        {
            case(NONE): fprintf(fpout, "%s is not a formula.  \n", formula);break;
            case(PROPOSITION): fprintf(fpout, "%s is a proposition. \n ", formula);break;
            case(NEGATION): fprintf(fpout, "%s is a negation.  \n", formula);break;
            case(BINARY):fprintf(fpout, "%s is a binary. The first part is %s and the second part is %s  \n", formula, partone(formula), parttwo(formula));break;
            default:fprintf(fpout, "What the f***!  ");
        }
        if (parse(formula)!= NONE)
        {
          t = (tableau*)malloc(sizeof(tableau));
          t->leftBranch = NULL;
          t->rightBranch = NULL;
          t->currentFormula = formula;
          complete(t);
          // printTableau(t);
          /* here you should initialise a theory S with one formula (formula) in it and then initialise a tableau t with on theory (S) in it*/
          /* then you should call a function that completes the tableau t*/
          if (closed(t)) { fprintf(fpout, "%s is not satisfiable.\n", formula); }
          else { fprintf(fpout, "%s is satisfiable.\n", formula); }
          freeTableau(t);
        }
        else {
          fprintf(fpout, "I told you, %s is not a formula.\n", formula);
        } 
    }

 
    fclose(fp);
    fclose(fpout);
    

  return(0);
}





        