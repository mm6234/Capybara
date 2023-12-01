// search.test.js

//Step 1: Need to do: npm install --save-dev jest jsdom
//TO DO: Load HTML file into JSDOM Environment, mock "search doctor API call" function, set up DOM elements
//need to run npm test, add to dependencies 

const fs = require('fs');
const path = require('path');
const { JSDOM } = require('jsdom');
//only use below line if creating search.js file
//const { searchDoctor } = require('./search');

// Load the HTML content into the jsdom environment
const htmlFilePath = path.resolve(__dirname, 'search.html');
const htmlContent = fs.readFileSync(htmlFilePath, 'utf-8');
const dom = new JSDOM(htmlContent);
global.document = dom.window.document;

// Import the JavaScript logic embedded in the HTML
require('./search.html');

// Jest testing
describe('searchDoctor', () => {
  test('it should display the correct information for doctorId = 3', async () => {
    // Mock the fetch function to simulate API calls
    global.fetch = jest.fn(() => Promise.resolve({
      json: () => Promise.resolve({
        "id": 3,
        "doctorName": "Capybara",
        "rating": 3.5,
        "submissions": 1834,
        "location": {
          "latitude": 3.222,
          "longitude": 78.43
        },
        "practiceKeywords": ["Ear", "Nose", "Throat"],
        "languagesSpoken": ["English"],
        "Insurance": ["Aetna"],
        "Other": {
          "streetAddress": "NYC"
        }
      }),
      ok: true,
    }));

    // Mock DOM elements
    document.body.innerHTML = `
      <input type="text" id="doctorId" placeholder="Enter Doctor ID">
      <button onclick="searchDoctor()">Search</button>
      <div id="result"></div>
    `;

    // Set the input value to doctorId 3
    document.getElementById('doctorId').value = '3';

    // Trigger the searchDoctor function
    await searchDoctor();

    // Check if the resultContainer content is as expected
    const resultContainer = document.getElementById('result');
    const expectedText = `Result for Doctor ID 3: {
      "id": 3,
      "doctorName": "Capybara",
      "rating": 3.5,
      "submissions": 1834,
      "location": {
        "latitude": 3.222,
        "longitude": 78.43
      },
      "practiceKeywords": ["Ear", "Nose", "Throat"],
      "languagesSpoken": ["English"],
      "Insurance": ["Aetna"],
      "Other": {
        "streetAddress": "NYC"
      }
    }`;

    expect(resultContainer.textContent).toBe(expectedText);
  });

});
