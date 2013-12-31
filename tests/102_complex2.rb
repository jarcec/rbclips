# Copyright 2013 Jarek Jarcec Cecho <jarcec@apache.org>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
require 'test/unit'
require './rbclips'

class Test_Complex1 < Test::Unit::TestCase

  include Clips
  include Base

  def teardown
    Base.reset
  end

  def make_older(human)
    human.age = human.age + 1
    human.birthday = "yesterday"
    assert human.save
  end

  def test_main
    # Constraints
    number = Constraint.new :type => :integer

    # Templates
    human = Template.new("human") do |t|
      t.slot :name
      t.slot :birthday
      t.slot :age, :constraint => number
    end
    assert human.save

    # Rules
    birthdays = Rule.new :birthday do |r|
      h = r.pattern human, :birthday => "today"
      r.rcall self, :make_older, h
    end
    assert birthdays.save

    # Facts
    assert Fact.new(human, :name => "Honza", :age => 22, :birthday => "yesterday").save
    assert Fact.new(human, :name => "Martin", :age => 22, :birthday => "today").save

    # Run
    Base.run

    # Chech
    Fact.all.each do |human|
      case human.name
        when "Honza" then assert_equal human.age, 22
        when "Martin" then assert_equal human.age, 23
      end
    end
  end
end
