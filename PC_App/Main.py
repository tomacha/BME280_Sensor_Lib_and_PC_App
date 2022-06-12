##
# @author: Krzysztof Półchłopek
# @date: 11.06.2022
#

from PyQt5.QtWidgets import QApplication
import sys
from GUI import MainGUI

## Main program function
def main():
    app = QApplication(sys.argv)
    window = MainGUI()
    window.show()
    app.exec_()


if __name__ == '__main__':
    main()