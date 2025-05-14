# QuickWheel Project

## Overview
This repository contains both the backend and frontend code for the QuickWheel application.

## Getting Started

### Prerequisites
- [Node.js](https://nodejs.org/) (v16 or above recommended)
- [npm](https://www.npmjs.com/) (comes with Node.js)

### Installation

#### 1. Clone the repository
```sh
git clone <your-repo-url>
cd quickwheel
```

#### 2. Install dependencies for Backend
```sh
cd Backend
npm install
```

#### 3. Install dependencies for Frontend
```sh
cd ../frontend
npm install
```

## Running the Application

### Backend
```sh
cd Backend
npm start
```

### Frontend
```sh
cd frontend
npm run dev
```

## Notes
- All dependencies are managed via `npm` and are listed in the respective `package.json` files in `Backend/` and `frontend/`.
- Do **not** commit `node_modules`, `dist`, `build`, or any lock files (`package-lock.json`, `yarn.lock`, `pnpm-lock.yaml`) to git. These are already ignored in `.gitignore`.
- Environment variables should be set in `.env` files, which are also ignored by git.

## Additional Information
- For any issues, please open an issue in this repository.
- For development, use the recommended extensions listed in `.vscode/extensions.json` if available.
