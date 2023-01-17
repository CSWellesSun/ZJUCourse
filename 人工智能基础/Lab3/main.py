# %%
# 导入棋盘文件
from board import Board
import datetime

# 初始化棋盘
board = Board()

# 打印初始化棋盘
board.display()

# %%
class HumanPlayer:
    """
    人类玩家
    """

    def __init__(self, color):
        """
        玩家初始化
        :param color: 下棋方，'X' - 黑棋，'O' - 白棋
        """
        self.color = color
    

    def get_move(self, board):
        """
        根据当前棋盘输入人类合法落子位置
        :param board: 棋盘
        :return: 人类下棋落子位置
        """
        # 如果 self.color 是黑棋 "X",则 player 是 "黑棋"，否则是 "白棋"
        if self.color == "X":
            player = "黑棋"
        else:
            player = "白棋"

        # 人类玩家输入落子位置，如果输入 'Q', 则返回 'Q'并结束比赛。
        # 如果人类玩家输入棋盘位置，e.g. 'A1'，
        # 首先判断输入是否正确，然后再判断是否符合黑白棋规则的落子位置
        while True:
            action = input(
                    "请'{}-{}'方输入一个合法的坐标(e.g. 'D3'，若不想进行，请务必输入'Q'结束游戏。): ".format(player,
                                                                                 self.color))

            # 如果人类玩家输入 Q 则表示想结束比赛
            if action == "Q" or action == 'q':
                return "Q"
            else:
                row, col = action[1].upper(), action[0].upper()

                # 检查人类输入是否正确
                if row in '12345678' and col in 'ABCDEFGH':
                    # 检查人类输入是否为符合规则的可落子位置
                    if action in board.get_legal_actions(self.color):
                        return action
                else:
                    print("你的输入不合法，请重新输入!")


# %%
# 导入随机包
import random

class RandomPlayer:
    """
    随机玩家, 随机返回一个合法落子位置
    """

    def __init__(self, color):
        """
        玩家初始化
        :param color: 下棋方，'X' - 黑棋，'O' - 白棋
        """
        self.color = color
        

    def random_choice(self, board):
        """
        从合法落子位置中随机选一个落子位置
        :param board: 棋盘
        :return: 随机合法落子位置, e.g. 'A1' 
        """
        # 用 list() 方法获取所有合法落子位置坐标列表
        action_list = list(board.get_legal_actions(self.color))

        # 如果 action_list 为空，则返回 None,否则从中选取一个随机元素，即合法的落子坐标
        if len(action_list) == 0:
            return None
        else:
            return random.choice(action_list)

    def get_move(self, board):
        """
        根据当前棋盘状态获取最佳落子位置
        :param board: 棋盘
        :return: action 最佳落子位置, e.g. 'A1'
        """
        if self.color == 'X':
            player_name = '黑棋'
        else:
            player_name = '白棋'
        print("请等一会，对方 {}-{} 正在思考中...".format(player_name, self.color))
        action = self.random_choice(board)
        return action

# %%
import random
import math
import copy
import datetime
import numpy as np
class MCTreeNode:
    """
    蒙特卡洛树
    """
    def __init__(self, parent, action, color):
        self.action = action
        self.parent = parent
        self.children = []
        self.winNum = 0
        self.visitNum = 0
        self.color = color
    

    def UCB1(self, c):
        UCBlist = np.empty(len(self.children))
        for index, child in enumerate(self.children):
            UCB = child.winNum / child.visitNum + c * math.sqrt(2 * math.log(self.visitNum) / child.visitNum)
            UCBlist[index] = UCB
        return self.children[np.argmax(UCBlist)]


    def Expand(self, actionList):
        exploredAction = [child.action for child in self.children]
        unexploredAction = []
        for action in actionList:
            if action not in exploredAction:
                unexploredAction.append(action)
        if len(unexploredAction) != 0:
            randomAction = random.choice(unexploredAction)
            if self.color == 'X':
                color = 'O'
            else :
                color = 'X'
            newNode = MCTreeNode(self, randomAction, color)
            newNode.visitNum = newNode.winNum = 0
            self.children.append(newNode)
            return newNode
        else:
            return None


    def BackPropagate(self, value):
        startNode = self
        while startNode:
            startNode.visitNum += 1
            if startNode.color == 'X':
                startNode.winNum += value
            elif startNode.color == 'O':
                startNode.winNum -= value
            startNode = startNode.parent
            

class UCTSearch:
    """
    蒙特卡洛搜索
    """
    def __init__(self, board, color, c):
        self.board = board
        self.color = color
        if color == 'X':
            self.root = MCTreeNode(None, None, 'O') # dummy head
        else:
            self.root = MCTreeNode(None, None, 'X')
        self.c = c


    def Search(self):
        start_time = datetime.datetime.now()
        end_time = datetime.datetime.now()
        value = 0
        while (end_time - start_time).seconds < 1:
            board = copy.deepcopy(self.board)
            currentNode = self.SelectPolicy(self.root, board)
            winner, diff = self.SimulatePolicy(board, currentNode.color)
            if (self.color == 'X' and winner == 0):
                value = diff
            elif (self.color == 'O' and winner == 1):
                value = -diff
            else:
                value = 0
            currentNode.BackPropagate(value)
            end_time = datetime.datetime.now()
        
        if len(self.root.children) != 0:
            return self.root.UCB1(self.c).action
        else:
            return None


    def SimulatePolicy(self, board, color):
        if color == 'X':
            color = 'O'
        else:
            color = 'X'
        actionList = list(board.get_legal_actions(color))
        if len(actionList) != 0:
            selectAction = random.choice(actionList)
            board._move(selectAction, color)
            return self.SimulatePolicy(board, color)
        else:
            return board.get_winner()
        

    def SelectPolicy(self, currentNode, board):
        selectNode = currentNode
        if selectNode.color == 'X':
            color = 'O'
        else:
            color = 'X'
        actionList = list(self.board.get_legal_actions(color))
        while actionList != None:
            expandNode = selectNode.Expand(actionList)
            if expandNode != None:
                board._move(expandNode.action, expandNode.color)
                return expandNode
            elif len(selectNode.children) != 0:
                selectNode =  selectNode.UCB1(self.c)
                board._move(selectNode.action, selectNode.color)
                actionList = list(self.board.get_legal_actions(selectNode.color)) 
            else:
                return selectNode

# %%
class AIPlayer:
    """
    AI 玩家
    """

    def __init__(self, color):
        """
        玩家初始化
        :param color: 下棋方，'X' - 黑棋，'O' - 白棋
        """

        self.color = color

    def get_move(self, board):
        """
        根据当前棋盘状态获取最佳落子位置
        :param board: 棋盘
        :return: action 最佳落子位置, e.g. 'A1'
        """
        if self.color == 'X':
            player_name = '黑棋'
        else:
            player_name = '白棋'
        print("请等一会，对方 {}-{} 正在思考中...".format(player_name, self.color))

        # -----------------请实现你的算法代码--------------------------------------

        
        action = None
        c = 2
        Model = UCTSearch(board, self.color, c)
        action = Model.Search()

        # ------------------------------------------------------------------------

        return action


# %%
# 导入黑白棋文件
from game import Game  

# 人类玩家黑棋初始化
black_player =  AIPlayer("X") # RandomPlayer("X")

# AI 玩家 白棋初始化
white_player = HumanPlayer("O") # AIPlayer("O")

# 游戏初始化，第一个玩家是黑棋，第二个玩家是白棋
game = Game(black_player, white_player)

# 开始下棋
game.run()

# %%



